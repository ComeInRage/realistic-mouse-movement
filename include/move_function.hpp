#include "common.hpp"
#include "ticks_generator.hpp"

#include <cassert>
#include <iostream>



namespace real_mouse
{
    template <concepts::moving_policy MovePolicy, concepts::trajectory Trajectory>
    void move(MovePolicy &&moving_policy, Trajectory &&trajectory)
    {
        point position;

        if constexpr (concepts::with_origin<Trajectory>)
        {
            position = trajectory.origin();
        }
        else
        {
            position = moving_policy.get_position();
        }

        for (auto sleeper = sleeping_iterator{}; sleeper != sleeping_iterator::sentinel{}; ++sleeper)
        {
            moving_policy.set_position(position);
            
            if constexpr (concepts::with_destination<Trajectory>)
            {
                if (position == trajectory.destination()) [[unlikely]]
                {
                    return;
                }
            }

            if constexpr (concepts::with_conditional_end<Trajectory>)
            {
                if (trajectory.is_end(position)) [[unlikely]]
                {
                    return;
                }
            }

            auto &&[next_point, time] = trajectory.next_point(position);

            position = next_point;
            sleeper.set_sleep_duration(std::chrono::duration_cast<sleeping_iterator::duration_type>(time));
        }
    }
}