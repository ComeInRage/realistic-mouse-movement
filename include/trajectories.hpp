#pragma once

#include "common.hpp"



namespace real_mouse // fwd
{
    class ticks_generator;
}

namespace real_mouse
{
    struct line
    {
        line(point from, point to, size_t velocity = 100) noexcept;

        [[nodiscard]] point origin();
        [[nodiscard]] point destination();
        [[nodiscard]] point next_point(ticks_generator& ticks, point current);

        point from;
        point to;
        size_t velocity;
        size_t tick_processed;
    };
}