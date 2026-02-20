#pragma once

#include <chrono>
#include <iterator>
#include <type_traits>



namespace real_mouse
{
    class sleeping_iterator
    {
    public:
        using clock_type = std::chrono::steady_clock;
        using time_point = clock_type::time_point;
        using duration_type = clock_type::duration;

        using value_type = duration_type;
        using difference_type = std::ptrdiff_t;
        using sentinel = std::default_sentinel_t;

    public:
        [[nodiscard]] value_type operator * ();

        sleeping_iterator& operator ++ ();
        [[nodiscard]] sleeping_iterator operator ++ (int);

        [[nodiscard]] bool operator == (sentinel) const noexcept;
        [[nodiscard]] bool operator != (sentinel) const noexcept;

        [[nodiscard]] time_point last_deref() const noexcept;

        void set_sleep_duration(duration_type duration) noexcept(std::is_nothrow_copy_assignable_v<duration_type>);

    private:
        duration_type m_sleep_duration = {};
        time_point m_last_incr = {};
        time_point m_prev_incr = {};
    };
}