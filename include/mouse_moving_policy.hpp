#pragma once

#include "common.hpp"

#ifdef _WIN32

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
    class mouse_moving_policy
    {
    public:
        void set_position(point position)
        {
#ifdef _WIN32
#ifndef NDEBUG
            ++m_set_position_count;
#endif
            assert(position.x <= std::numeric_limits<int>::max());
            assert(position.y <= std::numeric_limits<int>::max());

            if (!SetCursorPos(static_cast<int>(position.x), static_cast<int>(position.y)))
            {
                throw std::runtime_error{ "Unable to set cursor position" };
            }
#endif
        }

        [[nodiscard]] point get_position() const
        {
#ifdef _WIN32
            POINT pos{};

            if (!GetCursorPos(&pos))
            {
                throw std::runtime_error{ "Unable to get cursor position" };
            }

            assert(pos.x > 0 && pos.y > 0);
            return { static_cast<coord_type>(pos.x), static_cast<coord_type>(pos.y) };
#endif
        }

#ifndef NDEBUG
        [[nodiscard]] size_t set_positions_count() const noexcept
        {
            return m_set_position_count;
        }

    private:
        size_t m_set_position_count = 0;
#endif
    };
}