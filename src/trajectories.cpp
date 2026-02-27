#include "trajectories.hpp"
#include "mouse_moving_policy.hpp"



namespace real_mouse
{
    straight_uniform_motion::straight_uniform_motion(point from, point to, double velocity/* = 100 */) noexcept
        : m_from(from)
        , m_to(to)
        , m_velocity(velocity)
    {}

    point straight_uniform_motion::start_position() const noexcept
    {
        return m_from;
    }
}