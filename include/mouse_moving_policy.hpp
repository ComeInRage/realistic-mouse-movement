#pragma once

#include "common.hpp"

#include <thread>



namespace real_mouse
{
    template <typename Derived>
    class move_controller_base
    {
    public:
        template <concepts::trajectory Trajectory>
        void move(Trajectory &&trajectory);
        void set_position(point position) noexcept(std::is_nothrow_invocable_v<decltype(&Derived::set_position), Derived*, point>);

        [[nodiscard]] point get_position() const noexcept(std::is_nothrow_invocable_v<decltype(&Derived::get_position), Derived*>);
        [[nodiscard]] point get_last_position() const noexcept;

    private:
        [[nodiscard]] const Derived& to_derived() const noexcept;
        [[nodiscard]] Derived& to_derived() noexcept;

    private:
        mutable point m_last_known_pos;
    };

    template <typename Derived>
    template <concepts::trajectory Trajectory>
    void move_controller_base<Derived>::move(Trajectory &&trajectory)
    {
        static_assert(std::derived_from<Derived, move_controller_base<Derived>>);

        if constexpr (concepts::with_start_position<Trajectory>)
        {
            //set_position(trajectory.start_position());
            (void)get_position();
        }
        else
        {
            (void)get_position();
        }

        for (;;)
        {
            auto time_start = std::chrono::steady_clock::now();

            if (trajectory.is_ended(*this)) [[unlikely]] { return; }

            auto [next_point, time_to_reach] = trajectory.next_point(*this);
            auto sleep_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(time_to_reach);

            while (sleep_time > (std::chrono::steady_clock::now() - time_start));

            set_position(next_point);
        }
    }

    template <typename Derived>
    void move_controller_base<Derived>::set_position(point position) noexcept(std::is_nothrow_invocable_v<decltype(&Derived::set_position), Derived*, point>)
    {
        m_last_known_pos = position;
        to_derived().set_position(position);
    }

    template <typename Derived>
    point move_controller_base<Derived>::get_position() const noexcept(std::is_nothrow_invocable_v<decltype(&Derived::get_position), Derived*>)
    {
        m_last_known_pos = to_derived().get_position();
        return m_last_known_pos;
    }

    template <typename Derived>
    point move_controller_base<Derived>::get_last_position() const noexcept
    {
        return m_last_known_pos;
    }

    template <typename Derived>
    const Derived& move_controller_base<Derived>::to_derived() const noexcept
    {
        return static_cast<const Derived&>(*this);
    }

    template <typename Derived>
    Derived& move_controller_base<Derived>::to_derived() noexcept
    {
        return static_cast<Derived&>(*this);
    }

    struct mouse_controller : public move_controller_base<mouse_controller>
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