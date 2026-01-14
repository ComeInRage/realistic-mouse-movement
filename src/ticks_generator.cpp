#include "ticks_generator.hpp"

#include <cassert>



namespace real_mouse
{
    ticks_generator::ticks_count_iterator::ticks_count_iterator(duration_type tick_duration)
        : m_tick_duration(tick_duration)
    {}

    ticks_generator::ticks_count_iterator::value_type ticks_generator::ticks_count_iterator::operator * () const noexcept
    {
        auto last_deref_time = std::exchange(m_last_deref_time, clock_type::now());

        if (last_deref_time == time_point{}) [[unlikely]] { return 1; }

        auto ticks_passed = (m_last_deref_time - last_deref_time) / m_tick_duration;
        assert(ticks_passed >= 0);
        return static_cast<value_type>(ticks_passed);
    }

    ticks_generator::ticks_count_iterator& ticks_generator::ticks_count_iterator::operator ++ () noexcept
    {
        return *this;
    }

    ticks_generator::ticks_count_iterator ticks_generator::ticks_count_iterator::operator ++ (int) noexcept
    {
        return *this;
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