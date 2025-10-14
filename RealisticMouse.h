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
    namespace detail
    {
        class SynchronousMouseTasksQueue
        {
        public:
            using task_type = std::function<void()>;

        public:
            SynchronousMouseTasksQueue();

            SynchronousMouseTasksQueue(const SynchronousMouseTasksQueue&) = delete;
            SynchronousMouseTasksQueue(SynchronousMouseTasksQueue&&) = delete;

            SynchronousMouseTasksQueue& operator = (const SynchronousMouseTasksQueue&) = delete;
            SynchronousMouseTasksQueue& operator = (SynchronousMouseTasksQueue&&) = delete;

            ~SynchronousMouseTasksQueue();

        public:
            void add_task(task_type task);
            void block_and_wait() const;

            [[nodiscard]] bool is_running() const;

        private:
            void process_tasks();

        private:
            std::thread                     m_worker;
            std::queue<task_type>           m_tasks;
            std::condition_variable_any     m_newTaskWaiter;
            std::atomic_bool                m_terminate;

            mutable std::mutex                  m_tasksAdditionMutex;
            mutable std::shared_mutex           m_tasksModificationMutex;
            mutable std::condition_variable_any m_endTaskWaiter;
        };
    }

    namespace concepts
    {
        template <typename T>
        concept ExecutionPolicy = requires(T && policy, detail::SynchronousMouseTasksQueue::task_type task)
        {
            { policy.add_move_task(std::move(task)) };
            { policy.add_click_task(std::move(task)) };
            { policy.join_move_tasks() };
            { policy.join_click_tasks() };
            { policy.is_running() } -> std::convertible_to<bool>;
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

    namespace concepts
    {
        template <typename T>
        concept Point = requires (T && point)
        {
            { point.x } -> std::convertible_to<std::int32_t>;
            { point.y } -> std::convertible_to<std::int32_t>;
        };

        template <typename T>
        concept TrajectoryIterator = std::input_iterator<T> && requires (T && t)
        {
            { *t } -> Point;
        };

        template <typename T>
        concept Trajectory = std::ranges::range<T> && requires(T && t)
        {
            { t.begin() } -> TrajectoryIterator;
            { t.next() } -> Point;
        };

        template <typename T>
        concept TrajectoryTemplate = requires(T && t)
        {
            { t.make_trajectory() } -> Trajectory;
        };
    }

    struct Point
    {
        using coord_type = std::int32_t;

        coord_type x;
        coord_type y;
    };

    class Line
    {
    public:
        class iterator
        {
        public:
            [[nodiscard]] Point operator * () const noexcept { return current; }
            [[maybe_unused]] iterator& operator ++ () noexcept { next(); return *this; };
            [[nodiscard]] iterator operator ++ (int) noexcept { auto copy = *this; copy.next(); return copy; };

        private:
            void next();

        private:
            const Line* trajectory;
            Point current;
        };

    private:
        Point begin;
        Point end;
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