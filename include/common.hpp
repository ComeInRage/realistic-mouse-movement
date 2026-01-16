#pragma once

#include <cstdint>
#include <concepts>



namespace real_mouse
{
    using coord_type = size_t;

    struct point
    {
        coord_type x;
        coord_type y;
    };

    namespace concepts
    { 
        template <typename T>
        concept moving_policy = requires(T && policy)
        {
            { policy.set_position(coord_type{0}, coord_type{0}) };
            { policy.get_position() } -> std::convertible_to<point>;
        };

        template <typename T>
        concept trajectory = requires(T && t, point current)
        {
            { t.next_point(current) } -> std::convertible_to<point>;
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