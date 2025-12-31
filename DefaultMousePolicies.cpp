#pragma once

#ifdef _WIN32

#include "DefaultMousePolicies.hpp"
#include "RealisticMouse.hpp"

#include <limits>
#include <windows.h>



namespace real_mouse
{
    void MousePolicyWindows::push_up()
    {
        auto [x, y] = get_position();

        MOUSEINPUT mouseInput{ x, y, 0, (DWORD)MOUSEEVENTF_ABSOLUTE | (DWORD)MOUSEEVENTF_LEFTUP };
        INPUT input{ .type = INPUT_MOUSE, .mi = mouseInput };

        SendInput(1, &input, sizeof(input));
    }

    void MousePolicyWindows::push_down()
    {
        auto [x, y] = get_position();

        MOUSEINPUT mouseInput{ x, y, 0, (DWORD)MOUSEEVENTF_ABSOLUTE | (DWORD)MOUSEEVENTF_LEFTDOWN };
        INPUT input{ .type = INPUT_MOUSE, .mi = mouseInput };

        SendInput(1, &input, sizeof(input));
    }

    void MousePolicyWindows::click(std::chrono::milliseconds pressed_time)
    {
        push_down();
        std::this_thread::sleep_for(pressed_time);
        push_up();
    }

    void MousePolicyWindows::set_position(std::uint32_t x, std::uint32_t y)
    {
        if (x > std::numeric_limits<int>::max()) { throw std::runtime_error{}; }
        if (y > std::numeric_limits<int>::max()) { throw std::runtime_error{}; }

        SetCursorPos(static_cast<int>(x), static_cast<int>(y));
    }

    point_type MousePolicyWindows::get_position()
    {
        POINT pos{};
        
        if (!GetCursorPos(&pos))
        {
            throw std::runtime_error{};
        }

        return { pos.x, pos.y };
    }
}

#endif