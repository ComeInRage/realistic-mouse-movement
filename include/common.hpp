#pragma once

#include <cstdint>
#include <concepts>



namespace real_mouse // fwd
{
    class ticks_generator;
}

namespace real_mouse
{
    using coord_type = double;

    struct point
    {
        coord_type x;
        coord_type y;
    };

    [[nodiscard]] inline bool operator == (point lhs, point rhs) noexcept { return static_cast<std::ptrdiff_t>(lhs.x) == static_cast<std::ptrdiff_t>(rhs.x) && static_cast<std::ptrdiff_t>(lhs.y) == static_cast<std::ptrdiff_t>(rhs.y); }
    [[nodiscard]] inline bool operator != (point lhs, point rhs) noexcept { return !(lhs == rhs); }

    namespace concepts
    { 
        template <typename T>
        concept moving_policy = requires(T && policy, point position)
        {
            { policy.set_position(position) };
            { policy.get_position() } -> std::convertible_to<point>;
        };

        template <typename T>
        concept trajectory = requires(T && t, ticks_generator &gen, point current)
        {
            { t.next_point(gen, current) } -> std::convertible_to<point>;
        };

        template <typename T>
        concept with_origin = requires(T && t)
        {
            { t.origin() } -> std::convertible_to<point>;
        };

        template <typename T>
        concept with_destination = requires(T && t)
        {
            { t.destination() } -> std::convertible_to<point>;
        };
    }
}