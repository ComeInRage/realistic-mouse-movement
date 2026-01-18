#include "ticks_generator.hpp"

#include <cassert>
#include <thread>



namespace real_mouse
{
    ticks_generator::passed_ticks_count_iterator::passed_ticks_count_iterator(const ticks_generator &generator) noexcept(std::is_nothrow_default_constructible_v<time_point>)
        : m_generator(&generator)
        , m_last_deref_time()
    {}

    ticks_generator::passed_ticks_count_iterator::value_type ticks_generator::passed_ticks_count_iterator::operator * () const noexcept
    {
        assert(m_generator != nullptr);

        auto last_deref_time = std::exchange(m_last_deref_time, clock_type::now());

        if (last_deref_time == time_point{}) [[unlikely]] { return 1; }

        if (m_generator->m_tick_duration == duration_type{}) [[unlikely]] { return 1; }

        auto ticks_passed = (m_last_deref_time - last_deref_time) / m_generator->m_tick_duration;
        assert(ticks_passed >= 0);

        return static_cast<value_type>(ticks_passed);
    }

    ticks_generator::passed_ticks_count_iterator& ticks_generator::passed_ticks_count_iterator::operator ++ () noexcept
    {
        using namespace std::chrono_literals;

        assert(m_generator != nullptr);

        std::this_thread::sleep_for(m_generator->m_tick_duration);

        return *this;
    }

    ticks_generator::passed_ticks_count_iterator ticks_generator::passed_ticks_count_iterator::operator ++ (int) noexcept
    {
        operator ++();
        return *this;
    }

    bool ticks_generator::passed_ticks_count_iterator::operator == (sentinel) const noexcept
    {
        return false;
    }



    ticks_generator::ticks_generator(duration_type tick_duration/* = default_tick_duration*/) noexcept(std::is_nothrow_copy_constructible_v<duration_type>)
        : m_tick_duration(tick_duration)
    {}

    void ticks_generator::set_tick_duration(duration_type tick_duration) noexcept(std::is_nothrow_copy_assignable_v<duration_type>)
    {
        m_tick_duration = tick_duration;
    }

    ticks_generator::passed_ticks_count_iterator ticks_generator::begin() const noexcept(std::is_nothrow_constructible_v<time_point, const ticks_generator&>)
    {
        return { *this };
    }

    ticks_generator::sentinel ticks_generator::end() const noexcept
    {
        return {};
    }
}