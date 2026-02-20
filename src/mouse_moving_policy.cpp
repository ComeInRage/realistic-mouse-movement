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
#endif



namespace real_mouse
{
    void mouse_moving_policy::set_position(point position)
    {
#ifdef DEBUG_COUNTERS
        ++m_set_position_count;
#endif

#ifdef _WIN32
        assert(position.x <= std::numeric_limits<int>::max());
        assert(position.y <= std::numeric_limits<int>::max());

        if (!SetCursorPos(static_cast<int>(position.x), static_cast<int>(position.y)))
        {
            throw std::runtime_error{ "Unable to set cursor position" };
        }
#else
        static_assert(false, "Not implemented");
#endif
    }

    point mouse_moving_policy::get_position() const
    {
#ifdef _WIN32
        POINT pos{};

        if (!GetCursorPos(&pos))
        {
            throw std::runtime_error{ "Unable to get cursor position" };
        }

        assert(pos.x >= 0 && pos.y >= 0);
        return { static_cast<coord_type>(pos.x), static_cast<coord_type>(pos.y) };
#else
        static_assert(false, "Not implemented");
#endif
    }

    void mouse_moving_policy::push_down()
    {
#ifdef _WIN32
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
#endif
    }

    void mouse_moving_policy::push_up()
    {
#ifdef _WIN32
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
#endif
    }

#ifdef DEBUG_COUNTERS
    size_t mouse_moving_policy::set_positions_count() const noexcept
    {
        return m_set_position_count;
    }
#endif
}