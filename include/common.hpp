#pragma once

#include <chrono>
#include <cstdint>
#include <concepts>



namespace real_mouse
{
    struct point
    {
        double x;
        double y;
    };

    struct velocity_vec
    {
        double vx;
        double vy;
    };

    namespace concepts
    {
        namespace details // for concepts only
        {
            struct dummy_moving_policy;
            struct dummy_trajectory;

            struct dummy_moving_policy
            {
                void move(dummy_trajectory &); /* no-op */
                void set_position(point); /* no-op */
                point get_position() const; /* no-op */
                point get_last_position() const noexcept; /* no-op */
            };

            struct dummy_trajectory
            {
                std::optional<velocity_vec> approx_velocity(dummy_moving_policy &) const; /* no-op */
            };
        }

        template <typename T>
        concept approx_velocity = requires (T &&t)
        {
            { *t } -> std::convertible_to<velocity_vec>;
            { static_cast<bool>(t) } -> std::same_as<bool>;
        };

        template <typename T>
        concept approx_trajectory = requires(T && t, details::dummy_moving_policy &moving_policy)
        {
            { t.approx_velocity(moving_policy) } -> approx_velocity;
        };

        template <typename T>
        concept with_start_position = requires(T && t)
        {
            requires approx_trajectory<T>;

            { t.start_position() } -> std::convertible_to<point>;
        };

        template <typename T>
        concept moving_policy = requires(T && policy, details::dummy_trajectory &traj, point position)
        {
            { policy.move(traj) };
            { policy.set_position(position) };
            { policy.get_position() } -> std::convertible_to<point>;
            { policy.get_last_position() } -> std::convertible_to<point>;
        };

        template <typename T>
        concept mouse_control = requires(T && mouse)
        {
            requires moving_policy<T>;

            { mouse.push_up() };
            { mouse.push_down() };
        };
    }

    namespace arith
    {
        static constexpr size_t epsilon_factor = 10;

        [[nodiscard]] static bool dequal(double a, double b, size_t factor = epsilon_factor)
        {
            return std::fabs(a - b) < (std::numeric_limits<double>::epsilon() * factor);
        }

        [[nodiscard]] static bool dless(double a, double b)
        {
            return a < b && !dequal(a, b);
        }

        [[nodiscard]] static bool dgreater(double a, double b)
        {
            return a > b && !dequal(a, b);
        }
    }
}

[[nodiscard]] inline real_mouse::velocity_vec operator + (real_mouse::velocity_vec lhs, real_mouse::velocity_vec rhs) noexcept
{
    return { lhs.vx + rhs.vx, lhs.vy + rhs.vy };
}

[[nodiscard]] inline real_mouse::velocity_vec operator - (real_mouse::velocity_vec lhs, real_mouse::velocity_vec rhs) noexcept
{
    return { lhs.vx - rhs.vx, lhs.vy - rhs.vy };
}

template <typename VOpt>
    requires std::same_as<VOpt, std::optional<real_mouse::velocity_vec>>
[[nodiscard]] std::optional<real_mouse::velocity_vec> operator + (const VOpt &lhs, const VOpt &rhs) noexcept
{
    if (!lhs || !rhs) { return std::nullopt; }

    return std::optional<real_mouse::velocity_vec>{ std::in_place, *lhs + *rhs };
}