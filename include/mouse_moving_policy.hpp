#pragma once

#include "common.hpp"



namespace real_mouse
{
    template <typename Derived>
    class approx_moving_policy
    {
    public:
        template <concepts::approx_trajectory Trajectory>
        void move(Trajectory &&trajectory);
        void set_position(point position) noexcept(noexcept(to_derived().set_position(position)));

        [[nodiscard]] point get_position() const noexcept(noexcept(to_derived().get_position()));
        [[nodiscard]] point get_last_position() const noexcept;

    private:
        [[nodiscard]] const Derived& to_derived() const noexcept;
        [[nodiscard]] Derived& to_derived() noexcept;

    private:
        mutable point m_last_known_pos;
    };

    template <typename Derived>
    template <concepts::approx_trajectory Trajectory>
    void approx_moving_policy<Derived>::move(Trajectory &&trajectory)
    {
        static_assert(std::derived_from<Derived, approx_moving_policy<Derived>>);

        if constexpr (concepts::with_start_position<Trajectory>)
        {
            set_position(trajectory.start_position());
        }
        else
        {
            (void)get_position();
        }

        for (;;)
        {
            auto time_start = std::chrono::steady_clock::now();

            auto opt_velocity = trajectory.approx_velocity(*this);

            if (!opt_velocity) [[unlikely]] { break; }
            
            auto velocity = *opt_velocity;
            auto position = get_last_position();
            auto next_x = arith::dequal(velocity.vx, 0.) ? position.x : (velocity.vx > 0. ? std::trunc(position.x + 1) : std::trunc(position.x - 1));
            auto next_y = arith::dequal(velocity.vy, 0.) ? position.y : (velocity.vy > 0. ? std::trunc(position.y + 1) : std::trunc(position.y - 1));
            auto distance_to_next_x = next_x - position.x;
            auto distance_to_next_y = next_y - position.y;
            auto time_to_reach_next_x = arith::dequal(velocity.vx, 0.) ? 0. : distance_to_next_x / velocity.vx;
            auto time_to_reach_next_y = arith::dequal(velocity.vy, 0.) ? 0. : distance_to_next_y / velocity.vy;

            point next_point;
            std::chrono::steady_clock::duration sleep_time;

            if (arith::dequal(time_to_reach_next_x, time_to_reach_next_y))
            {
                next_point = { next_x, next_y };
                sleep_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(time_to_reach_next_x));
            }
            else if (time_to_reach_next_x > time_to_reach_next_y)
            {
                next_point = { next_x, position.y + time_to_reach_next_x * velocity.vy };
                sleep_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(time_to_reach_next_x));
            }
            else
            {
                next_point = { position.x + time_to_reach_next_y * velocity.vx, next_y };
                sleep_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(time_to_reach_next_y));
            }

            while (sleep_time > (std::chrono::steady_clock::now() - time_start));

            set_position(next_point);
        }
    }

    template <typename Derived>
    void approx_moving_policy<Derived>::set_position(point position) noexcept(noexcept(to_derived().set_position(position)))
    {
        m_last_known_pos = position;
        to_derived().set_position(position);
    }

    template <typename Derived>
    point approx_moving_policy<Derived>::get_position() const noexcept(noexcept(to_derived().get_position()))
    {
        m_last_known_pos = to_derived().get_position();
        return m_last_known_pos;
    }

    template <typename Derived>
    point approx_moving_policy<Derived>::get_last_position() const noexcept
    {
        return m_last_known_pos;
    }

    template <typename Derived>
    const Derived& approx_moving_policy<Derived>::to_derived() const noexcept
    {
        return static_cast<const Derived&>(*this);
    }

    template <typename Derived>
    Derived& approx_moving_policy<Derived>::to_derived() noexcept
    {
        return static_cast<Derived&>(*this);
    }

    struct mouse_controller : public approx_moving_policy<mouse_controller>
    {
        void set_position(point position);
        [[nodiscard]] point get_position() const;

        void push_down();
        void push_up();

#ifndef NDEBUG
        size_t position_changes_count = 0;
#endif
    };
}