#pragma once

#include "common.hpp"



namespace real_mouse
{
    class mouse_moving_policy
    {
    public:
        template <concepts::trajectory Trajectory>
        void move(Trajectory &&trajectory);
        void set_position(point position);
        [[nodiscard]] point get_position() const;

        void push_down();
        void push_up();

#ifndef NDEBUG
        [[nodiscard]] size_t get_position_changes_count() const noexcept;

    private:
        size_t m_set_position_count = 0;
#endif
    };

    template <concepts::trajectory Trajectory>
    void mouse_moving_policy::move(Trajectory &&trajectory)
    {
        point position;

        if constexpr (concepts::with_start_position<Trajectory>)
        {
            position = trajectory.start_position();
            set_position(position);
        }
        else
        {
            position = get_position();
        }

        for (;;)
        {
            auto time_start = std::chrono::steady_clock::now();

            if (trajectory.is_ended(*this, position)) [[unlikely]] { return; }

            auto [next_point, time_to_reach] = trajectory.next_point(*this, position);
            auto moving_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(time_to_reach);

            while (moving_duration > (std::chrono::steady_clock::now() - time_start));

            set_position(next_point);
            position = next_point;
        }
    }
}