#pragma once

#include <future>
#include <queue>
#include <shared_mutex>
#include <optional>
#include <chrono>

namespace real_mouse
{
    using namespace std::chrono_literals;
    
    namespace detail
    {
        namespace concepts
        {
            template <typename Fn, typename ...Args>
            concept Task = std::is_invocable_v<Fn, Args...>&& std::is_void_v<std::invoke_result_t<Fn, Args...>>;
        }

        class SynchoniousTasksQueue
        {
        private:
            using future_type = std::future<void>;

        public:
            SynchoniousTasksQueue();

            SynchoniousTasksQueue(const SynchoniousTasksQueue&) = delete;
            SynchoniousTasksQueue(SynchoniousTasksQueue&&) = delete;

            SynchoniousTasksQueue& operator = (const SynchoniousTasksQueue&) = delete;
            SynchoniousTasksQueue& operator = (SynchoniousTasksQueue&&) = delete;

            ~SynchoniousTasksQueue();

        public:
            template<typename Func, typename ...Args>
                requires concepts::Task<Func, Args...>
            void add_task(Func&& task, Args&& ...args);
            void block_and_wait() const;

            [[nodiscard]] bool is_running() const;

        private:
            void process_tasks();

        private:
            std::thread                     m_worker;
            std::queue<future_type>         m_tasks;
            std::condition_variable_any     m_newTaskWaiter;
            std::atomic_bool                m_terminate;
            
            mutable std::mutex                  m_tasksAdditionMutex;
            mutable std::shared_mutex           m_tasksModificationMutex;
            mutable std::condition_variable_any m_endTaskWaiter;
        };

        SynchoniousTasksQueue::SynchoniousTasksQueue()
            : m_worker{ &SynchoniousTasksQueue::process_tasks, this }
            , m_tasks{}
            , m_newTaskWaiter{}
            , m_terminate{}
            , m_tasksModificationMutex{}
        {}

        SynchoniousTasksQueue::~SynchoniousTasksQueue()
        {
            m_terminate = true;
            m_newTaskWaiter.notify_all();
            m_worker.join();
        }

        template<typename Func, typename ...Args>
            requires concepts::Task<Func, Args...>
        void SynchoniousTasksQueue::add_task(Func&& task, Args&& ...args)
        {
            {
                auto _ = std::scoped_lock(m_tasksAdditionMutex,
                                          m_tasksModificationMutex);

                m_tasks.emplace(std::async(std::launch::deferred,
                                           std::forward<Func>(task),
                                           std::forward<Args>(args)...));
            }

            m_newTaskWaiter.notify_one();
        }

        void SynchoniousTasksQueue::block_and_wait() const
        {
            auto lock = std::unique_lock(m_tasksModificationMutex, std::defer_lock);
            auto _ = std::scoped_lock(lock, m_tasksAdditionMutex);

            // m_tasksAdditionMutex is not unlocked here to block add_task function, until waiting ends
            m_endTaskWaiter.wait(lock, [this]() { return m_tasks.empty(); });
        }

        bool SynchoniousTasksQueue::is_running() const
        {
            auto _ = std::shared_lock(m_tasksModificationMutex);

            return !m_tasks.empty();
        }

        void SynchoniousTasksQueue::process_tasks()
        {
            while (!m_terminate)
            {
                auto lock = std::unique_lock(m_tasksModificationMutex);

                m_newTaskWaiter.wait(lock, [this] () { return m_terminate || !m_tasks.empty(); });

                if (m_terminate)
                {
                    m_tasks = {};
                    m_endTaskWaiter.notify_all();
                    break;
                }

                auto &&task = m_tasks.front();

                lock.unlock();
                task.wait(); // task execution
                lock.lock();

                m_tasks.pop();

                if (m_tasks.empty()) { m_endTaskWaiter.notify_all(); }
            }
        }
    }

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
        detail::SynchoniousTasksQueue m_movingTasks;
        detail::SynchoniousTasksQueue m_clickingTasks;
    };
}