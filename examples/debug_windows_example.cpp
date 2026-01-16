#include "move_function.hpp"

#include <iostream>
#include <windows.h>



#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif



namespace rm = real_mouse;

struct mouse_moving_policy
{
    void set_position(rm::point position);
    [[nodiscard]] rm::point get_position() const;
};

void mouse_moving_policy::set_position(rm::point position)
{
    assert(position.x < std::numeric_limits<int>::max());
    assert(position.y < std::numeric_limits<int>::max());

    if (!SetCursorPos(static_cast<int>(position.x), static_cast<int>(position.y)))
    {
        throw std::runtime_error{ "Unable to set cursor position" };
    }
}

rm::point mouse_moving_policy::get_position() const
{
    POINT pos{};

    if (!GetCursorPos(&pos))
    {
        throw std::runtime_error{ "Unable to get cursor position" };
    }

    assert (pos.x > 0 && pos.y > 0);
    return { static_cast<rm::coord_type>(pos.x), static_cast<rm::coord_type>(pos.y) };
}

struct line
{
    line(rm::point from, rm::point to, size_t velocity = 100) noexcept;

    [[nodiscard]] rm::point origin();
    [[nodiscard]] rm::point destination();
    [[nodiscard]] rm::point next_point(rm::ticks_generator &ticks, rm::point current);

    rm::point from;
    rm::point to;
    size_t velocity;
};

line::line(rm::point from, rm::point to, size_t velocity/* = 100 */) noexcept
    : from(from)
    , to(to)
    , velocity(velocity)
{}

rm::point line::origin()
{
    return from;
}

rm::point line::destination()
{
    return to;
}

rm::point line::next_point(rm::ticks_generator &ticks, rm::point current)
{
    using namespace std::chrono_literals;

    auto distance_x = to.x - from.x;
    auto distance_y = to.y - from.y;
    auto max_module = std::max(std::fabs(distance_x), std::fabs(distance_y));

    auto delta_per_tick_x = distance_x / max_module;
    auto delta_per_tick_y = distance_y / max_module;

    ticks.set_tick_duration(rm::ticks_generator::duration_type{ 1s } / velocity);

    return { current.x + delta_per_tick_x, current.y + delta_per_tick_y };
}

int main()
{
    using namespace std::chrono_literals;

    mouse_moving_policy moving_policy;

    rm::move(moving_policy, line{ { 100., 100. }, { 300., 300. }, 100 });

    return 0;
}
