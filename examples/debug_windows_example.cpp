#include "move_function.hpp"
#include "trajectories.hpp"
#include "mouse_moving_policy.hpp"

#include <thread>
#include <iostream>



namespace rm = real_mouse;

int main()
{
    using namespace std::literals;
    using namespace std::chrono_literals;

    std::this_thread::sleep_for(3s);

    rm::mouse_moving_policy moving_policy;
    auto trajectory = rm::line{ { 500., 500. }, { 700., 500. }, 100 };

    rm::move(moving_policy, trajectory);

    std::cout << std::format("Mouse moved in {} ticks"sv, trajectory.tick_processed);

    assert(trajectory.tick_processed == 200);

    return 0;
}
