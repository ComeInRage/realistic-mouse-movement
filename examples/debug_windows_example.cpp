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

    auto velocity = 1'00.;

    rm::mouse_moving_policy moving_policy;
    auto trajectory = rm::line{ { 200., 200. }, { 1000., 300. }, velocity };

    auto before = std::chrono::steady_clock::now();
    moving_policy.move(trajectory);
    auto after = std::chrono::steady_clock::now();

    std::cout << std::format("Mouse moved {} times.\nTime elapsed: {}"sv,
                             moving_policy.get_position_changes_count(),
                             std::chrono::duration_cast<std::chrono::milliseconds>(after - before)) << std::endl;

    return 0;
}
