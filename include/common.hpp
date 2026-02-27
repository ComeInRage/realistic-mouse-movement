#pragma once

#include <chrono>
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

    namespace concepts
    {
        namespace details // for concepts only
        {
            struct dummy_moving_policy;
            struct dummy_trajectory;

            struct dummy_moving_policy
            {
                [[deprecated]] void move(dummy_trajectory &); /* no-op */
                [[deprecated]] void set_position(point); /* no-op */
                [[deprecated]] point get_position() const; /* no-op */
                [[deprecated]] point get_last_position() const noexcept; /* no-op */
            };

            struct dummy_trajectory
            {
                [[deprecated]] std::pair<point, std::chrono::nanoseconds> next_point(dummy_moving_policy &) const; /* no-op */
                [[deprecated]] bool is_ended(dummy_moving_policy &) const noexcept; /* no-op */
            };
        }

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
        concept trajectory = requires(T && t, details::dummy_moving_policy &moving_policy)
        {
            { t.next_point(moving_policy) } -> next_point_result;
            { t.is_ended(moving_policy) } -> std::convertible_to<bool>;
        };

        template <typename T>
        concept with_start_position = requires(T && t)
        {
            requires trajectory<T>;

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