#include "common.hpp"
#include "ticks_generator.hpp"



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

        for (auto ticks : ticks_generator{})
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

                current_position = trajection.next_point(current_position);
            }
        }
    }
}