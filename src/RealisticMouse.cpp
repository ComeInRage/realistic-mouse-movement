#include <windows.h>
#include <algorithm>
#include <random>
#include <utility>
#include <numbers>
#include <thread>

#include "Core.h"
#include "Mouse.h"

namespace eve
{
  Mouse& Mouse::Init() noexcept
  {
    static Mouse mouse{};
    return mouse;
  }

  Coords Mouse::GetPosition() noexcept
  {
    POINT pos{};
    if (!GetCursorPos(&pos))
    {
      UnknownError;
    }
    return Coords{ pos.x, pos.y };
  }

  void Mouse::Click(Buttons button/* = Buttons::LEFT*/, std::chrono::milliseconds clickDuration/* = 100ms*/)
  {
    namespace thr = std::this_thread;

    std::lock_guard guard{ m_clickMutex };

    if (m_isClicking)
    {
      return;
    }

    m_isClicking = true;

    auto t = std::thread([this, button, clickDuration]() // all objects are cheap-to-copy
                         {
                           PushDown();
                           thr::sleep_for(clickDuration);
                           PushUp();
                           m_isClicking = false;
                         });
    t.detach();
  }

  void Mouse::Move(std::int32_t destX, std::int32_t destY)
  {
    namespace ch = std::chrono;
    static constexpr double velocity = 1500.; // pixels per second
    
    constexpr auto distPerCycle = 1.;
    constexpr ch::nanoseconds timeout{static_cast<std::int64_t>((distPerCycle / velocity) * 1000000)}; // amount of time spend to one pixel in nanoseconds
    
    double d = 1. / 1500.;

    auto [startX, startY] = GetPosition();
    auto remainDistance = std::hypot(destX - startX, destY - startY);
    auto ratio = remainDistance / distPerCycle;

    auto distPerCycleX = (destX - startX) / ratio;
    auto distPerCycleY = (destY - startY) / ratio;

    double currX = startX;
    double currY = startY;
    
    while (remainDistance > 1)
    {
      currX += distPerCycleX;
      currY += distPerCycleY;

      remainDistance = std::hypot(destX - currX, destY - currY);
      ratio          = remainDistance / distPerCycle;
      distPerCycleX  = (destX - currX) / ratio;
      distPerCycleY  = (destY - currY) / ratio;

      SetPosition(static_cast<std::int32_t>(currX),
                  static_cast<std::int32_t>(currY));
      std::this_thread::sleep_for(timeout);
    }

    SetPosition(destX, destY);
  }

  void Mouse::PushDown(Buttons button/* = Buttons::LEFT*/)
  {
    auto coords = GetPosition();
    DWORD buttonEvent = (button == Buttons::LEFT ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN);
    MOUSEINPUT mouseInput{ coords.x, coords.y, 0, (DWORD)MOUSEEVENTF_ABSOLUTE | buttonEvent };
    INPUT input{ .type = INPUT_MOUSE, .mi = mouseInput }; // Careful! mi is a member of the anonimous union
    SendInput(1, &input, sizeof(input));
  }

  void Mouse::PushUp(Buttons button/* = Buttons::LEFT*/)
  {
    auto coords = GetPosition();
    DWORD buttonEvent = (button == Buttons::LEFT ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP);
    MOUSEINPUT mouseInput{ coords.x, coords.y, 0, (DWORD)MOUSEEVENTF_ABSOLUTE | buttonEvent };
    INPUT input{ .type = INPUT_MOUSE, .mi = mouseInput }; // Careful! mi is a member of the anonimous union
    SendInput(1, &input, sizeof(input));
  }

  void Mouse::RealisticMove(std::int32_t destX, std::int32_t destY, std::int32_t velocity/* = 1000*/)
  {
    // Сделать ремарку о вдохновлении Wind Mouse
    // Сделать возможность конфигурирования
    static const double sqrt3             = std::sqrt(3); // Темп затухания скорости
    static const double sqrt5             = std::sqrt(5); // Ограничение нарастания скорости

    constexpr double windMag              = 1;  // Магнитуда случайных колебаний
    constexpr double gravity              = 1.5;  // Магнитуда гравитации
    constexpr std::int32_t dampDistance   = 50; // Дистанция, на которой случайные колебания прекращаются
    constexpr std::int32_t maxProjection  = 2;  // Максимальное значение проекции вектора перемещения

    auto [currentX, currentY]  = GetPosition();
    auto remainDist            = std::hypot(destX - currentX, destY - currentY);
    std::chrono::nanoseconds iterTimeout{ static_cast<std::int64_t>(remainDist / (velocity * remainDist) * 1000000) };

    auto windForce = [sqrt3=sqrt3, sqrt5=sqrt5, windMag=windMag, damp=dampDistance](double dist, double prevX = 0, double prevY = 0)
                       -> std::pair<double, double>
                     {
                       auto mag = std::min(windMag, dist);
                       std::random_device rd{};
                       std::mt19937 mt{ rd() };
                       std::uniform_real_distribution distribution{-1., 1.};

                       // Затухание происходит в любом случае
                       double x = prevX / sqrt3;
                       double y = prevY / sqrt3;

                       // Если дистанция больше, чем дистанция затухания, то случайным образом меняем отклонение
                       if (dist > damp)
                       {
                         x += distribution(mt) * mag / sqrt5;
                         y += distribution(mt) * mag / sqrt5;
                       }
                       return { x, y };
                     };

    auto gravityForce = [g=gravity, destX=destX, destY=destY](double dist, std::int32_t currX, std::int32_t currY)
                          -> std::pair<double, double>
                        {
                          auto gravityVecX = (destX - currX) / dist;
                          auto gravityVecY = (destY - currY) / dist;
                          double x         = g * gravityVecX;
                          double y         = g * gravityVecY;
                          return { x, y };
                        };

    double currStepX = 0;
    double currStepY = 0;
    double wX        = 0;
    double wY        = 0;
    double gX        = 0;
    double gY        = 0;
    double stepX     = 0;
    double stepY     = 0;

    while (remainDist > 1)
    {
      auto wind = windForce(remainDist, wX, wY);
      wX = wind.first;
      wY = wind.second;

      auto grav = gravityForce(remainDist, currentX, currentY);
      gX = grav.first;
      gY = grav.second;

      stepX += wX + gX;
      stepY += wY + gY;

      if (stepX > maxProjection || stepX < -maxProjection)
      {
        auto ratio = std::abs(maxProjection / stepX);
        stepX      = stepX * ratio;
        stepY      = stepY * ratio;
      }
      if (stepY > maxProjection || stepY < -maxProjection)
      {
        auto ratio = std::abs(maxProjection / stepY);
        stepX = stepX * ratio;
        stepY = stepY * ratio;
      }

#ifndef NDEBUG
      if (   stepX > maxProjection || stepX < -maxProjection
          || stepY > maxProjection || stepY < -maxProjection)
      {
        throw std::runtime_error("Too long step");
      }
#endif

      Move(currentX + static_cast<std::int32_t>(stepX), currentY + static_cast<std::int32_t>(stepY));
      WaitForMove();

      double _ = 0;
      stepX = std::modf(stepX, &_);
      stepY = std::modf(stepY, &_);

      auto currentPos = GetPosition();
      currentX   = currentPos.x;
      currentY   = currentPos.y;
      remainDist = std::hypot(destX - currentX, destY - currentY);

      std::this_thread::sleep_for(iterTimeout);
    }

    Move(destX, destY);
  }

  void Mouse::RealisticMove(Coords coords, std::int32_t velocity/* = 1000*/)
  {
    RealisticMove(coords.x, coords.y, velocity);
  }

  void Mouse::SetPosition(std::int32_t x, std::int32_t y)
  {
    SetCursorPos(x, y);
  }

  void Mouse::SetPosition(Coords coords)
  {
    SetPosition(coords.x, coords.y);
  }

  void Mouse::WaitForMove() const
  {
    
  }
}