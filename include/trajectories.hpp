#pragma once

#include "common.hpp"

#include <cassert>



namespace real_mouse
{
    class straight_uniform_motion
    {
    public:
        using duration = std::chrono::duration<double>;

    public:
        straight_uniform_motion(point from, point to, double velocity = 100) noexcept;

    public:
        template <concepts::moving_policy Moving>
        [[nodiscard]] std::pair<point, duration> next_point(Moving &&moving_policy) const;

        template <concepts::moving_policy Moving>
        [[nodiscard]] bool is_ended(Moving &&moving_policy) const noexcept;

        [[nodiscard]] point start_position() const noexcept;

    private:
        point m_from;
        point m_to;
        double m_velocity;
    };

    template <concepts::moving_policy Moving>
    std::pair<point, straight_uniform_motion::duration> straight_uniform_motion::next_point(Moving &&moving_policy) const
    {
        using namespace std::chrono_literals;

        static constexpr auto calc_next = [](double current_x, double current_y, double distance_x, double distance_y, double velocity) -> std::pair<point, std::chrono::duration<double>>
        {
            assert(distance_x != 0);

            auto delta_x = std::trunc(current_x + 1) - current_x;
            auto delta_y = (distance_y * delta_x) / distance_x;
            auto next = point{ current_x + delta_x, current_y + delta_y };

            // If next point will be reached on y-axis earlier, than on x-axis;
            if (static_cast<std::ptrdiff_t>(current_y) != static_cast<std::ptrdiff_t>(next.y))
            {
                auto new_delta_y = std::trunc(next.y) - current_y;
                auto similarity_factor = delta_y / new_delta_y;
                assert(similarity_factor != 0);

                delta_y = new_delta_y;
                delta_x = delta_x / similarity_factor;

                next.y = current_y + delta_y;
                next.x = current_x + delta_x;
            }

            auto distance_to_next_point = std::sqrt(delta_x * delta_x + delta_y * delta_y);
            auto time = std::chrono::duration<double>{ distance_to_next_point / velocity };

            return { next, time };
        };

        auto current = moving_policy.get_last_position();

        if (arith::dequal(m_velocity, 0))
        {
            return { current, {} };
        }

        auto distance_x = m_to.x - m_from.x;
        auto distance_y = m_to.y - m_from.y;

        if (arith::dless(distance_x, 1) && arith::dless(distance_y, 1))
        {
            return { current, {} };
        }

        if (arith::dgreater(std::fabs(distance_y), std::fabs(distance_x)))
        {
            auto res = calc_next(current.y, current.x, distance_y, distance_x, m_velocity);
            std::swap(res.first.x, res.first.y);
            return res;
        }

        return calc_next(current.x, current.y, distance_x, distance_y, m_velocity);
    }

    template <concepts::moving_policy Moving>
    bool straight_uniform_motion::is_ended(Moving &&moving_policy) const noexcept
    {
        auto position = moving_policy.get_last_position();
        return static_cast<std::ptrdiff_t>(position.x) == static_cast<std::ptrdiff_t>(m_to.x)
            && static_cast<std::ptrdiff_t>(position.y) == static_cast<std::ptrdiff_t>(m_to.y);
    }
}