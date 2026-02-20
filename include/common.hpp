#pragma once

#include "ticks_generator.hpp"

#include <cstdint>
#include <concepts>



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
        struct is_duration {};

        template <typename Rep, typename Rat>
        struct is_duration<std::chrono::duration<Rep, Rat>> { static constexpr bool value = true; };

        template <typename T>
        static constexpr bool is_duration_v = is_duration<T>::value;

        template <typename T>
        concept duration = is_duration_v<std::remove_reference_t<T>>;

        template <typename T>
        concept next_point_result = requires (T &&t)
        {
            { t.first } -> std::convertible_to<point>;
            { t.second } -> duration;
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

        template <typename T>
        concept with_conditional_end = requires(T && t, point current)
        {
            { t.is_end(current) } -> std::convertible_to<point>;
        };

        template <typename T>
        concept trajectory = requires(T && t, point current)
        {
            requires with_destination<T> || with_conditional_end<T>;

            { t.next_point(current) } -> next_point_result;
        };
    }

    namespace arith
    {
        [[nodiscard]] static bool dequal(double a, double b) noexcept
        {
            return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
        }

        [[nodiscard]] static bool dless(double a, double b) noexcept
        {
            return a < b && !dequal(a, b);
        }

        [[nodiscard]] static bool dgreater(double a, double b) noexcept
        {
            return a > b && !dequal(a, b);
        }
    }
}