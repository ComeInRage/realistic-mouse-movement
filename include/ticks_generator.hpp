#pragma once

#include <chrono>
#include <iterator>
#include <type_traits>



namespace real_mouse
{
    class ticks_generator
    {
    public:
        using clock_type = std::chrono::steady_clock;
        using time_point = clock_type::time_point;
        using duration_type = clock_type::duration;

    private:
        class passed_ticks_count_iterator
        {
        public:
            using value_type = size_t;
            using difference_type = std::ptrdiff_t;
            using sentinel = std::default_sentinel_t;

        public:
            passed_ticks_count_iterator(const ticks_generator &generator) noexcept(std::is_nothrow_default_constructible_v<time_point>);

        public:
            [[nodiscard]] value_type operator * () const noexcept;

            passed_ticks_count_iterator& operator ++ () noexcept;
            [[nodiscard]] passed_ticks_count_iterator operator ++ (int) noexcept;

            [[nodiscard]] bool operator == (sentinel) const noexcept;

        private:
            const ticks_generator * m_generator;
            mutable time_point m_last_deref_time;
        };

    public:
        using iterator = passed_ticks_count_iterator;
        using sentinel = iterator::sentinel;

    private:
        static constexpr duration_type default_tick_duration = std::chrono::milliseconds{ 1 };

    public:
        ticks_generator(duration_type tick_duration = default_tick_duration) noexcept(std::is_nothrow_copy_constructible_v<duration_type>);
        
        ticks_generator(const ticks_generator&) = delete;
        ticks_generator(ticks_generator&&) = delete;
        ticks_generator& operator = (const ticks_generator&) = delete;
        ticks_generator& operator = (ticks_generator&&) = delete;

        ~ticks_generator() = default;

    public:
        void set_tick_duration(duration_type tick_duration) noexcept(std::is_nothrow_copy_assignable_v<duration_type>);

        [[nodiscard]] passed_ticks_count_iterator begin() const noexcept(std::is_nothrow_constructible_v<time_point, const ticks_generator&>);
        [[nodiscard]] sentinel end() const noexcept;

    private:
        duration_type m_tick_duration;
    };
}