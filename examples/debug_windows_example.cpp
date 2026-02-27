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

    std::this_thread::sleep_for(3s);

    auto velocity = 10000.;

    rm::mouse_controller moving_policy;
    auto trajectory = rm::straight_uniform_motion{ { 200., 200. }, { 0., 0. }, velocity };

    auto before = std::chrono::steady_clock::now();
    moving_policy.move(trajectory);
    auto after = std::chrono::steady_clock::now();

    std::cout << std::format("Mouse moved {} times.\nTime elapsed: {}"sv,
                             moving_policy.position_changes_count,
                             std::chrono::duration_cast<std::chrono::milliseconds>(after - before)) << std::endl;

    return 0;
}
