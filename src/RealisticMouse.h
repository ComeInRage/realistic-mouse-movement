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

    // TODO for multithreading
    //void   WaitForMove() const;

  private:
    Mouse() = default;

    enum class NearestPixel : std::int8_t
    {
      NO_ONE      = 0,
      TOP         = 1,
      TOP_RIGHT   = 2,
      RIGHT       = 3,
      LOWER_RIGHT = 4,
      LOWER       = 5,
      LOWER_LEFT  = 6,
      LEFT        = 7,
      TOP_LEFT    = 8
    };

    std::mutex m_clickMutex;
    bool m_isClicking = false;
  };
}