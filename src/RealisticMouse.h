#pragma once

#include <chrono>
#include <mutex>

namespace real_mouse
{
  using namespace std::chrono_literals;

  // Class that represents the computer mouse.
  class Mouse
  {
  public:
    // Enum that represents mouse's buttons.
    // Note, that mouse wheel is not provided now.
    enum class Buttons
    {
      LEFT = 0,
      RIGHT = 1,
    };

    Mouse(const Mouse&)            = delete;
    Mouse(Mouse&&)                 = delete;
    Mouse& operator=(const Mouse&) = delete;
    Mouse& operator=(Mouse&&)      = delete;
    ~Mouse()                       = default;

    // Mouse class can has only one object.
    // In that cause Mouse can be obtained only by this fabric method.
    static Mouse& Init() noexcept;

    // Returns current coordinates. Don't require Mouse object creation.
    static std::pair<std::int32_t, std::int32_t> GetPosition();

    // Simulates mouse click on current coordinates.
    // Takes the mouse button and the time during which mouse button will be pressed.
    void   Click(Buttons button = Buttons::LEFT, std::chrono::milliseconds clickDuration = 100ms);

    // Moves mouse to given coordinates.
    // Note that, unlike SetPosition(....), this function moves mouse smoothly.
    void   Move(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    // Pushes down given mouse button and holds one in pressed state.
    void   PushDown(Buttons button = Buttons::LEFT);

    // Pushes up given mouse button and holds one in unpressed state.
    void   PushUp(Buttons button = Buttons::LEFT);

    // Moves mouse to given coordinates.
    // Note that, unlike SetPosition(....), this function moves mouse smoothly.
    void   RealisticMove(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    // Sets mouse coordinates to given ones.
    // Note that, unlike Move(....), this function instantly sets coordinates.
    void   SetPosition(std::int32_t x, std::int32_t y);

    Mouse& WaitForClick() const;

    // TODO for multithreading
    //void   WaitForMove() const;

    //// Observers

    bool   IsClicking() const noexcept;

  private:
    Mouse() = default;

    // This primitive incapsulates the logic of working with std::condition_variable.
    // All methods are thread-safe.
    // Can be transferred out of class.
    struct SyncPrimitive
    {
    public:
      [[maybe_unused]] inline SyncPrimitive& LockAndBlock()
      {
        std::unique_lock lock{ mutex };
        locked = true;
        condition.wait(lock, [this]() { return !locked; });
        return *this;
      }

      [[maybe_unused]] inline SyncPrimitive& LockOrBlock()
      {
        std::unique_lock lock{ mutex };
        if (locked)
        {
          condition.wait(lock, [this](){ return !locked; });
        }
        else
        {
          locked = true;
        }
        return *this;
      }

      [[maybe_unused]] inline SyncPrimitive& Unlock(bool notifyAll = false)
      {
        std::lock_guard guard{ mutex };
        locked = false;

        if (notifyAll)
        {
          condition.notify_all();
        }
        else
        {
          condition.notify_one();
        }

        return *this;
      }

      [[nodiscard]] inline bool IsLocked()
      {
        std::lock_guard guard{ mutex };
        return locked;
      }

    private:
      std::condition_variable condition{};
      std::mutex              mutex{};
      bool                    locked{ false };
    };

    mutable SyncPrimitive m_clickPrimitive;
  };
}