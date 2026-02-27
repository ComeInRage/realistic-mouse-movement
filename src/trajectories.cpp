#include "trajectories.hpp"
#include "mouse_moving_policy.hpp"



namespace real_mouse
{
    straight_uniform_motion::straight_uniform_motion(point to, double velocity/* = 100 */) noexcept
        : m_to(to)
        , m_velocity(velocity)
    {}
}