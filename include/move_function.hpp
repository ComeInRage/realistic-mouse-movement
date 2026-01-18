#include "common.hpp"
#include "ticks_generator.hpp"

#include <cassert>



namespace real_mouse
{
    template <concepts::moving_policy MovePolicy, concepts::trajectory Trajectory>
    void move(MovePolicy &&moving_policy, Trajectory &&trajectory)
    {
        auto generator = ticks_generator{};
        auto position = moving_policy.get_position();

        if constexpr (concepts::with_origin<Trajectory>)
        {
            position = trajectory.origin();
        }

        for (auto ticks : generator)
        {
            assert(ticks > 0);

            for (; ticks; --ticks)
            {
                moving_policy.set_position(position);

                if constexpr (concepts::with_destination<Trajectory>)
                {
                    if (position == trajectory.destination())
                    {
                        return;
                    }
                }

                auto &&[next_point, time] = trajectory.next_point(position);

                position = next_point;
                generator.set_tick_duration(std::chrono::duration_cast<ticks_generator::duration_type>(time));
            }
        }
    }
}