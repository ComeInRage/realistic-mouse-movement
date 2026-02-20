#pragma once

#include "common.hpp"



namespace real_mouse
{
    class mouse_moving_policy
    {
    public:
        void set_position(point position);

        [[nodiscard]] point get_position() const;

        void push_down();
        void push_up();

#ifdef DEBUG_COUNTERS
        [[nodiscard]] size_t set_positions_count() const noexcept;

    private:
        size_t m_set_position_count = 0;
#endif
    };
}