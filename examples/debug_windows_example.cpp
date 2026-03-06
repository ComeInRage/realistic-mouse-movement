#include "trajectories.hpp"
#include "mouse_moving_policy.hpp"

#include <thread>
#include <iostream>



#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif



namespace rm = real_mouse;

int main()
{
    using namespace std::literals;
    using namespace std::chrono_literals;

    rm::mouse_controller moving_policy;
    auto trajectory = rm::straight_uniform_motio{ .dest = rm::point{ 1000., 0. }, .velocity = 1000. }
                    | rm::straight_uniform_motio{ .dest = rm::point{ 0., 1000. }, .velocity = 1000. };

    auto before = std::chrono::steady_clock::now();
    moving_policy.move(trajectory);
    auto after = std::chrono::steady_clock::now();

    std::cout << std::format("Mouse moved {} times.\nTime elapsed: {}"sv,
#ifndef NDEBUG
                             moving_policy.position_changes_count,
#else
                             0,
#endif
                             std::chrono::duration_cast<std::chrono::milliseconds>(after - before)) << std::endl;

    return 0;
}
