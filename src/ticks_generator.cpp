#include "ticks_generator.hpp"

#include <cassert>



namespace real_mouse
{
    ticks_generator::ticks_count_iterator::ticks_count_iterator(duration_type tick_duration)
        : m_tick_duration(tick_duration)
    {}

    ticks_generator::ticks_count_iterator::value_type ticks_generator::ticks_count_iterator::operator * () const noexcept
    {
        if (m_since == time_point{}) { return 1; }

        auto ticks_passed = (m_now - m_since) / m_tick_duration;
        assert(ticks_passed >= 0);
        return static_cast<value_type>(ticks_passed);
    }

    ticks_generator::ticks_count_iterator& ticks_generator::ticks_count_iterator::operator ++ ()
    {
        m_since = m_now;
        m_now = ticks_generator::clock_type::now();

        return *this;
    }

    ticks_generator::ticks_count_iterator ticks_generator::ticks_count_iterator::operator ++ (int)
    {
        auto copy = *this;

        operator++();

        return copy;
    }

    bool ticks_generator::ticks_count_iterator::operator == (sentinel) const noexcept
    {
        return false;
    }



    ticks_generator::ticks_generator(duration_type tick_duration)
        : m_tick_duration(tick_duration)
    {}

    ticks_generator::ticks_count_iterator ticks_generator::begin() const noexcept
    {
        return { m_tick_duration };
    }

    ticks_generator::sentinel ticks_generator::end() const noexcept
    {
        return {};
    }
}