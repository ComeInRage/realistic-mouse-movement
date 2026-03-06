#include "common.hpp"
#include "mouse_moving_policy.hpp"


#ifdef _WIN32
#include <limits>
#include <cassert>
#include <stdexcept>
#include <windows.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace real_mouse
{
    void mouse_controller::set_position(point position)
    {
        assert(position.x <= std::numeric_limits<int>::max());
        assert(position.y <= std::numeric_limits<int>::max());

        if (!SetCursorPos(static_cast<int>(position.x), static_cast<int>(position.y)))
        {
            throw std::runtime_error{ "Unable to set cursor position" };
        }

#ifndef NDEBUG
        ++position_changes_count;
#endif
    }

    point mouse_controller::get_position() const
    {
        POINT pos{};

        if (!GetCursorPos(&pos))
        {
            throw std::runtime_error{ "Unable to get cursor position" };
        }

        assert(pos.x >= 0 && pos.y >= 0);
        return { static_cast<double>(pos.x), static_cast<double>(pos.y) };
    }

    void mouse_controller::push_down()
    {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
    }

    void mouse_controller::push_up()
    {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}

#else

namespace real_mouse
{
    void mouse_controller::set_position(point position)
    {
        static_assert(false, "Not implemented yet");

#ifndef NDEBUG
        ++position_changes_count;
#endif
    }

    point mouse_controller::get_position() const
    {
        static_assert(false, "Not implemented yet");
    }

    void mouse_controller::push_down()
    {
        static_assert(false, "Not implemented yet");
    }

    void mouse_controller::push_up()
    {
        static_assert(false, "Not implemented yet");
    }
}

#endif