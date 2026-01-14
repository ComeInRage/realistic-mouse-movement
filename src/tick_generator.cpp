#include "tick_generator.hpp"



namespace real_mouse
{
    tick_generator::ticks_count_iterator::ticks_count_iterator(duration_type tick_duration)
        : m_tick_duration(tick_duration)
    {}

    tick_generator::ticks_count_iterator::value_type tick_generator::ticks_count_iterator::operator * () const noexcept
    {
        if (m_since == time_point{}) { return 1; }

        return (m_now - m_since) / m_tick_duration;
    }

    tick_generator::ticks_count_iterator& tick_generator::ticks_count_iterator::operator ++ ()
    {
        m_since = m_now;
        m_now = tick_generator::clock_type::now();

        return *this;
    }

    tick_generator::ticks_count_iterator tick_generator::ticks_count_iterator::operator ++ (int)
    {
        auto copy = *this;

        operator++();

        return copy;
    }

    bool tick_generator::ticks_count_iterator::operator == (sentinel) const noexcept
    {
        return true;
    }



    tick_generator::tick_generator(duration_type tick_duration)
        : m_tick_duration(tick_duration)
    {}

    tick_generator::ticks_count_iterator tick_generator::begin() const noexcept
    {
        return { m_tick_duration };
    }

    tick_generator::sentinel tick_generator::end() const noexcept
    {
        return {};
    }
}