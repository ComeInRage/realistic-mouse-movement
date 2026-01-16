#include "common.hpp"
#include "ticks_generator.hpp"

#include <cassert>



namespace real_mouse
{
    template <concepts::moving_policy MovePolicy, concepts::trajectory Trajectory>
    void move(MovePolicy &&moving_policy, Trajectory &&trajectory)
    {
        if constexpr (concepts::with_origin<Trajectory>)
        {
            moving_policy.set_position(trajectory.origin());
        }

        auto current_position = moving_policy.get_position();
        auto generator = ticks_generator{};

        for (auto ticks : generator)
        {
            assert(ticks > 0);

            for (; ticks; --ticks)
            {
                if constexpr (concepts::with_destination<Trajectory>)
                {
                    if (current_position == trajectory.destination())
                    {
                        return;
                    }
                }

                current_position = trajectory.next_point(generator, current_position);
                moving_policy.set_position(current_position);
            }
        }
    }
}