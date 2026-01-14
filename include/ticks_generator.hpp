#pragma once

#include <chrono>
#include <iterator>



namespace real_mouse
{
    class ticks_generator
    {
    private:
        using clock_type = std::chrono::steady_clock;
        using time_point = clock_type::time_point;
        using duration_type = clock_type::duration;

        class ticks_count_iterator
        {
        public:
            using value_type = size_t;
            using difference_type = std::ptrdiff_t;
            using sentinel = std::default_sentinel_t;

        public:
            ticks_count_iterator(duration_type tick_duration);

        public:
            [[nodiscard]] value_type operator * () const noexcept;

            ticks_count_iterator& operator ++ () noexcept;
            [[nodiscard]] ticks_count_iterator operator ++ (int) noexcept;

            [[nodiscard]] bool operator == (sentinel) const noexcept;

        private:
            duration_type m_tick_duration;
            mutable time_point m_last_deref_time;
        };

    public:
        using iterator = ticks_count_iterator;
        using sentinel = iterator::sentinel;

    private:
        static constexpr duration_type default_tick_duration = std::chrono::microseconds{ 1 };

    public:
        ticks_generator(duration_type tick_duration = default_tick_duration);

    public:
        [[nodiscard]] ticks_count_iterator begin() const noexcept;
        [[nodiscard]] sentinel end() const noexcept;

    private:
        duration_type m_tick_duration;
    };
}