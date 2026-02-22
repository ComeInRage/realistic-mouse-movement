#pragma once

#include "common.hpp"



namespace real_mouse //fwd
{
    class mouse_moving_policy;
}

namespace real_mouse
{
    class line
    {
    public:
        line(point from, point to, double velocity = 100) noexcept;

    public:
        [[nodiscard]] point start_position() const noexcept;
        [[nodiscard]] bool is_ended(mouse_moving_policy &moving_policy, point position) const noexcept;
        [[nodiscard]] std::pair<point, std::chrono::duration<double>> next_point(mouse_moving_policy &moving_policy, point position) const;

    private:
        point m_from;
        point m_to;
        double m_velocity;
    };
}