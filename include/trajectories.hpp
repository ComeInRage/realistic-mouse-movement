#pragma once

#include "move_function.hpp"



namespace real_mouse
{
    class line
    {
    public:
        line(point from, point to, double velocity = 100) noexcept;

    public:
        [[nodiscard]] point origin();
        [[nodiscard]] point destination();
        [[nodiscard]] std::pair<point, std::chrono::duration<double>> next_point(point current);
        [[nodiscard]] size_t ticks_processed() const noexcept;

    private:
        point m_from;
        point m_to;
        double velocity;

#ifndef NDEBUG
        size_t m_ticks_processed;
#endif
    };
}