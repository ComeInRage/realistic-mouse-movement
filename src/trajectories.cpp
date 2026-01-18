#include "trajectories.hpp"



namespace real_mouse
{
    line::line(point from, point to, double velocity/* = 100 */) noexcept
        : m_from(from)
        , m_to(to)
        , velocity(velocity)
        , m_ticks_processed(0)
    {}

    point line::origin()
    {
        return m_from;
    }

    point line::destination()
    {
        return m_to;
    }

    std::pair<point, std::chrono::duration<double>> line::next_point(point current)
    {
        using namespace std::chrono_literals;

        auto distance_x = m_to.x - m_from.x;
        auto distance_y = m_to.y - m_from.y;
        auto distance_y_is_greater = std::fabs(distance_y) > std::fabs(distance_x);

        if (distance_y_is_greater)
        {
            std::swap(distance_x, distance_y);
        }

        auto delta_x = std::trunc(current.x + 1) - current.x;
        auto delta_y = (distance_y * delta_x) / distance_x;

        auto next = point{ current.x + delta_x, current.y + delta_y };

        // Moving to the next point on the y-axis
        if (static_cast<std::ptrdiff_t>(current.y) != static_cast<std::ptrdiff_t>(next.y))
        {
            auto new_delta_y = std::trunc(next.y) - current.y;
            auto similarity_factor = delta_y / new_delta_y;

            delta_y = new_delta_y;
            delta_x = delta_x / similarity_factor;

            next.y = current.y + delta_y;
            next.x = current.x + delta_x;
        }

        auto distance_to_next_point = std::sqrt(delta_x * delta_x + delta_y * delta_y);
        auto time = std::chrono::duration<double>{ distance_to_next_point / velocity };

        if (distance_y_is_greater)
        {
            std::swap(next.x, next.y);
        }

#ifndef NDEBUG
        ++m_ticks_processed;
#endif

        return { next, time };
    }

    size_t line::ticks_processed() const noexcept
    {
        return m_ticks_processed;
    }
}