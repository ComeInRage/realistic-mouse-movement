#pragma once

#include "common.hpp"

#include <cassert>



namespace real_mouse
{
    template <typename Derived>
    struct approx_trajectory
    {
        [[nodiscard]] const Derived& as_derived() const noexcept;
        [[nodiscard]] Derived& as_derived() noexcept;

        template <concepts::moving_policy Moving>
        [[nodiscard]] std::optional<velocity_vec> approx_velocity(Moving &moving_policy) noexcept(noexcept(as_derived().approx_velocity(moving_policy)));
    };

    template <typename Derived>
    const Derived& approx_trajectory<Derived>::as_derived() const noexcept
    {
        return static_cast<const Derived&>(*this);
    }

    template <typename Derived>
    Derived& approx_trajectory<Derived>::as_derived() noexcept
    {
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    template <concepts::moving_policy Moving>
    std::optional<velocity_vec> approx_trajectory<Derived>::approx_velocity(Moving &moving_policy) noexcept(noexcept(as_derived().approx_velocity(moving_policy)))
    {
        static_assert(std::derived_from<Derived, approx_trajectory<Derived>>);
        return as_derived().approx_derived(moving_policy);
    }
}

namespace real_mouse
{
    struct straight_uniform_motio : public approx_trajectory<straight_uniform_motio>
    {
        template <concepts::moving_policy Moving>
        [[nodiscard]] std::optional<velocity_vec> approx_velocity(Moving &moving_policy) noexcept(noexcept(moving_policy.get_last_position()));

        point dest;
        double velocity;
    };

    template <concepts::moving_policy Moving>
    std::optional<velocity_vec> straight_uniform_motio::approx_velocity(Moving &moving_policy) noexcept(noexcept(moving_policy.get_last_position()))
    {
        auto position = moving_policy.get_last_position();
        auto dist_x = dest.x - position.x;
        auto dist_y = dest.y - position.y;
        auto distance = std::sqrt(dist_x * dist_x + dist_y * dist_y);

        if (arith::dequal(distance, 0.)) [[unlikely]] { return std::nullopt; }

        assert(arith::dgreater(distance, 0.));

        auto similarity_factor = velocity / distance;
        auto velocity_x = dist_x * similarity_factor;
        auto velocity_y = dist_y * similarity_factor;
        return std::optional<velocity_vec>(std::in_place, velocity_x, velocity_y);
    }
}

namespace real_mouse
{
    namespace details
    {
        template <typename T, typename ...Ts>
        struct trajectory_closure final : public approx_trajectory<trajectory_closure<T, Ts...>>
        {
            static constexpr auto make_accumulator = [](auto &moving_policy) noexcept
            {
                return [&](auto &t, auto &...ts) noexcept(noexcept((t.approx_velocity(moving_policy) + ... + ts.approx_velocity(moving_policy)))) -> std::optional<velocity_vec>
                       {
                           return (t.approx_velocity(moving_policy) + ... + ts.approx_velocity(moving_policy));
                       };
            };

            template <concepts::moving_policy Moving>
            [[nodiscard]] std::optional<velocity_vec> approx_velocity(Moving &&moving_policy) noexcept(noexcept(std::apply(make_accumulator(moving_policy), trajectories)));
            
            std::tuple<T, Ts...> trajectories;
        };

        template <typename T, typename ...Ts>
        template <concepts::moving_policy Moving>
        [[nodiscard]] std::optional<velocity_vec> trajectory_closure<T, Ts...>::approx_velocity(Moving &&moving_policy) noexcept(noexcept(std::apply(make_accumulator(moving_policy), trajectories)))
        {
            return std::apply(make_accumulator(moving_policy), trajectories);
        }

        template <typename T, typename ...Ts>
        trajectory_closure(std::tuple<T, Ts...> &&) -> trajectory_closure<T, Ts...>;

        template <typename T>
        struct is_closure : std::false_type {};

        template <typename T, typename ...Ts>
        struct is_closure<trajectory_closure<T, Ts...>> : std::true_type {};

        template <typename T>
        constexpr bool is_closure_v = is_closure<T>::value;
    }

    namespace concepts
    {
        template <typename T>
        concept closure = approx_trajectory<T> && ::real_mouse::details::is_closure_v<T>;
    }
}

template <real_mouse::concepts::approx_trajectory L, real_mouse::concepts::approx_trajectory R>
auto operator | (L &&lhs, R &&rhs) noexcept(noexcept(std::make_tuple(std::forward<L>(lhs), std::forward<L>(rhs))))
{
    return real_mouse::details::trajectory_closure{ .trajectories = std::make_tuple(std::forward<L>(lhs), std::forward<L>(rhs)) };
}

template <real_mouse::concepts::closure C, real_mouse::concepts::approx_trajectory T>
auto operator | (C &&closure, T &&trajectory) noexcept(noexcept(std::tuple_cat(std::forward<C>(closure).trajectories, std::forward<T>(trajectory))))
{
    return real_mouse::details::trajectory_closure{ .trajectories = std::tuple_cat(std::forward<C>(closure).trajectories, std::make_tuple(std::forward<T>(trajectory))) };
}

template <real_mouse::concepts::approx_trajectory T, real_mouse::concepts::closure C>
auto operator | (T &&trajectory, C &&closure) noexcept(noexcept(std::tuple_cat(std::forward<T>(trajectory), std::forward<C>(closure).trajectories)))
{
    return real_mouse::details::trajectory_closure{ .trajectories = std::tuple_cat(std::make_tuple(std::forward<T>(trajectory)), std::forward<C>(closure).trajectories) };
}

template <real_mouse::concepts::closure L, real_mouse::concepts::closure R>
auto operator | (L &&left_closure, R &&right_closure) noexcept(noexcept(std::tuple_cat(std::forward<L>(left_closure).trajectories, std::forward<R>(right_closure).trajectories)))
{
    return real_mouse::details::trajectory_closure{ .trajectories = std::tuple_cat(std::forward<L>(left_closure).trajectories, std::forward<R>(right_closure).trajectories) };
}