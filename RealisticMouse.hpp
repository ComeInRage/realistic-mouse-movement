#pragma once

#include <future>
#include <queue>
#include <shared_mutex>
#include <optional>
#include <chrono>
#include <concepts>
#include <ranges>

namespace real_mouse
{
    using coord_type = std::uint32_t;

    struct point_type
    {
        coord_type x;
        coord_type y;
    };

    namespace concepts
    {
        template <typename T>
        concept MousePolicy = requires(T && policy)
        {
            { policy.push_up() };
            { policy.push_down() };
            { policy.click() };
            { policy.set_position(size_t{0}, size_t{0}) };
            { policy.get_position() } -> std::convertible_to<point_type>;
        };

        template <typename T>
        concept ExecutionPolicy = requires(T && policy, std::function<void()> task)
        {
            { policy.do_move(std::move(task)) };
            { policy.do_click(std::move(task)) };
            { policy.wait_for_move() };
            { policy.wait_for_click() };
            { policy.wait() };
        };
    }

    class ParallelPolicy
    {
    public:
        using task_type = detail::SynchronousMouseTasksQueue::task_type;

    public:
        void add_move_task(task_type task);
        void add_click_task(task_type task);
        void join_move_tasks() const;
        void join_click_tasks() const;
        [[nodiscard]] bool is_running() const;

    private:
        detail::SynchronousMouseTasksQueue m_moves;
        detail::SynchronousMouseTasksQueue m_clicks;
    };

    class SequencedPolicy
    {
    public:
        using task_type = detail::SynchronousMouseTasksQueue::task_type;

    public:
        void add_move_task(task_type task);
        void add_click_task(task_type task);
        void join_move_tasks() const;
        void join_click_tasks() const;
        [[nodiscard]] bool is_running() const;

    private:
        void add_task(task_type task);
        void join_tasks() const;

    private:
        detail::SynchronousMouseTasksQueue m_tasks;
    };

    namespace execution
    {
        extern ParallelPolicy par;
        extern SequencedPolicy seq;
    }

    struct Point
    {
        using coord = std::int32_t;
        
        coord x;
        coord y;
    };

    struct Vector
    {
        using length = double;
        
        length x;
        length y;
    };

    template <typename Derived>
    class TrajectoryInterface
    {
    public:
        using point_type = Point;
        using vector_type = Vector;
        using ratio_type = double;

    public:
        [[nodiscard]] decltype(auto) begin() const noexcept { static_assert(std::derived_from<Derived, TrajectoryInterface<Derived>>); return static_cast<const Derived&>(*this).begin(); }
        [[nodiscard]] decltype(auto) end() const noexcept { return static_cast<const Derived&>(*this).end(); }
        
        [[nodiscard]] decltype(auto) begin() noexcept { return static_cast<Derived&>(*this).begin(); }
        [[nodiscard]] decltype(auto) end() noexcept { return static_cast<Derived&>(*this).end(); }

        [[nodiscard]] ratio_type ratio() const noexcept { return m_ratio; }
        [[nodiscard]] vector_type next(point_type current) noexcept { return static_cast<Derived&>(*this).next(current); }

    private:
        ratio_type m_ratio;
    };

    class Line : public TrajectoryInterface<Line>
    {
    private:
        using base_type = TrajectoryInterface;

    public:
        Line(point_type dest, ratio_type ratio) noexcept;

        Line() = delete;
        Line(const Line&) = default;
        Line(Line&&) = default;
        Line& operator = (const Line&) = default;
        Line& operator = (Line&&) = default;
        ~Line() = default;

    public:
        [[nodiscard]] point_type destination() const noexcept;
        [[nodiscard]] vector_type next(point_type current) const noexcept;
    
    private:
        point_type m_dest;
    };

    using namespace std::chrono_literals;

    // Class that represents the computer mouse.
    class Mouse
    {
    public:
        // Enum that represents mouse's buttons.
        // Note, that mouse wheel is not provided now.
        enum class Button
        {
            LEFT = 0,
            RIGHT = 1,
        };

    public:
        Mouse(const Mouse&) = delete;
        Mouse(Mouse&&) = delete;

        Mouse& operator=(const Mouse&) = delete;
        Mouse& operator=(Mouse&&) = delete;

        ~Mouse() = default;

    public:
        // Mouse is singleton.
        static Mouse& Instance();

        // Returns current coordinates. Don't require Mouse object creation.
        static std::pair<std::int32_t, std::int32_t> GetPosition();

    public:
        // Simulates mouse click on current coordinates.
        // Takes the mouse button and the time during which mouse button will be pressed.
        [[maybe_unused]] Mouse& click(Button button = Button::LEFT, std::chrono::milliseconds duration = 100ms);

        // Moves mouse to given coordinates.
        // Note that, unlike set_position(....), this function moves mouse smoothly.
        [[maybe_unused]] Mouse& move(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

        // Pushes down given mouse button and holds one in pressed state.
        [[maybe_unused]] Mouse& push_down(Button button = Button::LEFT);

        // Pushes up given mouse button and holds one in unpressed state.
        [[maybe_unused]] Mouse& push_up(Button button = Button::LEFT);

        // Moves mouse to given coordinates.
        // Note that, unlike set_position(....), this function moves mouse smoothly.
        [[maybe_unused]] Mouse& realistic_move(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

        // Sets mouse coordinates to given ones.
        // Note that, unlike move(....), this function instantly sets coordinates.
        [[maybe_unused]] Mouse& set_position(std::int32_t x, std::int32_t y);

        // Blocks calling thread, until all click() operations become finished.
        [[maybe_unused]] const Mouse& wait_clicks() const;

        // Blocks calling thread, until all click() operations become finished.
        [[maybe_unused]] Mouse& wait_clicks();

        // Blocks calling thread, until all moving operations become finished.
        [[maybe_unused]] const Mouse& wait_moves() const;

        // Blocks calling thread, until all moving operations become finished.
        [[maybe_unused]] Mouse& wait_moves();

        // Returns true if any thread processing click operation (here is underlying operations counter). False otherwise.
        [[nodiscard]] bool is_clicking() const;

        //Returns true if any thread processing moving operation (include realistic_move and similar). False otherwise.
        [[nodiscard]] bool is_moving() const;

    private:
        Mouse() = default;

    private:
        void move_impl(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);
        void realistic_move_impl(std::int32_t x, std::int32_t y, std::int32_t velocity = 1000);

    private:
        detail::SynchronousMouseTasksQueue m_movingTasks;
        detail::SynchronousMouseTasksQueue m_clickingTasks;
    };
}