#include "trajectories.hpp"
#include "ticks_generator.hpp"



namespace real_mouse
{
    line::line(point from, point to, size_t velocity/* = 100 */) noexcept
        : from(from)
        , to(to)
        , velocity(velocity)
        , tick_processed(0)
    {
    }

    point line::origin()
    {
        return from;
    }

    point line::destination()
    {
        return to;
    }

    point line::next_point(ticks_generator& ticks, point current)
    {
        using namespace std::chrono_literals;

        auto distance_x = to.x - from.x;
        auto distance_y = to.y - from.y;
        auto max_module = std::max(std::fabs(distance_x), std::fabs(distance_y));

        auto delta_per_tick_x = distance_x / max_module;
        auto delta_per_tick_y = distance_y / max_module;

        ticks.set_tick_duration(ticks_generator::duration_type{ 1s } / velocity);

        ++tick_processed;

        return { current.x + delta_per_tick_x, current.y + delta_per_tick_y };
    }
}