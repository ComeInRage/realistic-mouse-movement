#pragma once



namespace real_mouse // fwd
{
    struct point;
}

namespace real_mouse
{
    struct mouse_moving_policy
    {
        void set_position(point position);
        [[nodiscard]] point get_position() const;
    };
}