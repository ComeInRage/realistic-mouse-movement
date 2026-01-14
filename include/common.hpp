#pragma once

#include <cstdint>
#include <concepts>



namespace real_mouse
{
    using coord_type = size_t;

    struct point_type
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
            { policy.get_position() } -> std::convertible_to<point_type>;
        };
    }
}