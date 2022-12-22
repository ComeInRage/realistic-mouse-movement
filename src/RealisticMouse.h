#pragma once

#include <chrono>
#include <mutex>
#include <iostream>

namespace real_mouse
{
  using namespace std::chrono_literals;

  // This primitive incapsulates the logic of working with std::condition_variable.
  // For more information check methods descriptions.
  // Be careful! It is undefined behavior, if any thread works with SyncPrimitive after it's destructor was called.
  class SyncPrimitive
  {
  public:
    SyncPrimitive() = default;
    SyncPrimitive(const SyncPrimitive&) = delete;
    SyncPrimitive(SyncPrimitive&&) = delete;
    SyncPrimitive& operator=(const SyncPrimitive&) = delete;
    SyncPrimitive& operator=(SyncPrimitive&&) = delete;
    ~SyncPrimitive() = default;

    [[maybe_unused]] inline SyncPrimitive& BlockUntilLock()
    {
      std::unique_lock lock{ mutex };
      if (!locked)
      {
        condition.wait(lock, [this]() { return locked; });
      }
      return *this;
    }

    [[maybe_unused]] inline SyncPrimitive& BlockUntilUnlock()
    {
      std::unique_lock lock{ mutex };
      if (locked)
      {
        condition.wait(lock, [this]() { return !locked; });
      }
      return *this;
    }

    [[maybe_unused]] inline SyncPrimitive& BlockUntilUnlockAll()
    {
      std::unique_lock lock{ mutex };
      if (locksCount)
      {
        condition.wait(lock, [this]() { return !locksCount; });
      }
      return *this;
    }

    [[maybe_unused]] inline SyncPrimitive& LockOrBlock()
    {
      std::unique_lock lock{ mutex };
      ++locksCount;
      if (locked)
      {
        condition.wait(lock, [this]() { return !locked; });
      }
      locked = true;
      condition.notify_all();
      return *this;
    }

    [[maybe_unused]] inline SyncPrimitive& Unlock(bool notifyAll)
    {
      std::lock_guard guard{ mutex };
      locked = false;
      --locksCount;

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

    [[nodiscard]] inline bool IsAnyLocked()
    {
      std::lock_guard guard{ mutex };
      return locksCount;
    }

  private:
    std::condition_variable condition{};
    std::mutex              mutex{};
    size_t                  locksCount{ 0 };
    bool                    locked{ false };
  };

  // Blocks std::this_thread on exit out of scope, if underliyng SyncPrimitive is still unlocked.
  // This allows you to make sure that at least one thread locked his underlying SyncPrimitive.
  // Be careful! It is DEADLOCK, if no threads are available to lock ScopeBlockerGuard after it's destructor called.
  class ScopeBlockerGuard
  {
  public:
    ScopeBlockerGuard(SyncPrimitive &primitive) : m_primitive{ primitive } {}
    ScopeBlockerGuard()                                    = delete;
    ScopeBlockerGuard(const ScopeBlockerGuard&)            = delete;
    ScopeBlockerGuard(ScopeBlockerGuard&&)                 = delete;
    ScopeBlockerGuard& operator=(const ScopeBlockerGuard&) = delete;
    ScopeBlockerGuard& operator=(ScopeBlockerGuard&&)      = delete;
    ~ScopeBlockerGuard() { m_primitive.BlockUntilLock(); }

  private:
    SyncPrimitive& m_primitive;
  };

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
    static Mouse& Init();

    // Returns current coordinates. Don't require Mouse object creation.
    static std::pair<std::int32_t, std::int32_t> GetPosition();

    // Simulates mouse click on current coordinates.
    // Takes the mouse button and the time during which mouse button will be pressed.
    [[maybe_unused]] Mouse&            Click(Buttons button = Buttons::LEFT, std::chrono::milliseconds clickDuration = 100ms);

    // Moves mouse to given coordinates.
    // Note that, unlike SetPosition(....), this function moves mouse smoothly.
    [[maybe_unused]] Mouse&            Move(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    // Pushes down given mouse button and holds one in pressed state.
    [[maybe_unused]] Mouse&            PushDown(Buttons button = Buttons::LEFT);

    // Pushes up given mouse button and holds one in unpressed state.
    [[maybe_unused]] Mouse&            PushUp(Buttons button = Buttons::LEFT);

    // Moves mouse to given coordinates.
    // Note that, unlike SetPosition(....), this function moves mouse smoothly.
    [[maybe_unused]] Mouse&            RealisticMove(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    // Sets mouse coordinates to given ones.
    // Note that, unlike Move(....), this function instantly sets coordinates.
    [[maybe_unused]] Mouse&            SetPosition(std::int32_t x, std::int32_t y);

    // TODO Description
    [[maybe_unused]] const Mouse&      WaitForClick() const;

    // TODO description
    [[maybe_unused]] const Mouse&      WaitForMove() const;

    //// Observers

    [[nodiscard]]    bool              IsClicking() const;

    [[nodiscard]]    bool              IsMoving() const;

  private:
    Mouse() = default;

    void              MoveImpl(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);
    void              RealisticMoveImpl(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    mutable SyncPrimitive m_clickPrimitive;
    mutable SyncPrimitive m_movePrimitive; // This primitive is used by all versions of Move methods. Including RealisticMove
  };
}