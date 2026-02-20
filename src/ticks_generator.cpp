#include "ticks_generator.hpp"

#include <cassert>
#include <thread>



namespace real_mouse
{
    sleeping_iterator::value_type sleeping_iterator::operator * ()
    {
        return m_last_incr - m_prev_incr;
    }

    sleeping_iterator& sleeping_iterator::operator ++ ()
    {
        if (m_last_incr == time_point{}) [[unlikely]]
        {
            m_last_incr = clock_type::now();
        }

        while (m_sleep_duration > (clock_type::now() - m_last_incr));

        m_prev_incr = std::exchange(m_last_incr, clock_type::now());
        return *this;
    }

    sleeping_iterator sleeping_iterator::operator ++ (int)
    {
        operator++();
        return *this;
    }

    bool sleeping_iterator::operator == (sentinel) const noexcept
    {
        return false;
    }

    bool sleeping_iterator::operator != (sentinel) const noexcept
    {
        return !(*this == sentinel{});
    }

    sleeping_iterator::time_point sleeping_iterator::last_deref() const noexcept
    {
        return m_last_incr;
    }

    void sleeping_iterator::set_sleep_duration(duration_type duration) noexcept(std::is_nothrow_copy_assignable_v<duration_type>)
    {
        m_sleep_duration = duration;
    }
}