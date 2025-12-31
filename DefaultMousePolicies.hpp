#pragma once

#ifdef _WIN32

#include <cstdint>
#include <chrono>



namespace real_mouse // fwd
{
    struct point_type;
}

namespace real_mouse
{
    struct MousePolicyWindows
    {
        void push_up();
        void push_down();

        void click(std::chrono::milliseconds pressed_time);
        
        void set_position(std::uint32_t x, std::uint32_t y);
        [[nodiscard]] point_type get_position();
        
    };
}

#endif