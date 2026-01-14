#include "tasks_queue.hpp"



namespace real_mouse::details
{
    TasksQueue::TasksQueue()
        : m_processor{ &TasksQueue::process_tasks, this }
        , m_tasks{}
        , m_tasks_after_join{}
        , m_terminate{}
        , m_is_joined{}
        , m_tasks_addition_mutex{}
        , m_new_task_waiter{}
        , m_join_waiter{}
    {}

    TasksQueue::~TasksQueue()
    {
        m_terminate.store(true, std::memory_order_release);
        m_new_task_waiter.notify_all();
        m_join_waiter.notify_all();
        m_processor.join();
    }

    void TasksQueue::add_task(task_type task)
    {
        {
            auto _ = std::unique_lock(m_tasks_addition_mutex);
        
            if (m_is_joined)
            {
                m_tasks_after_join.push(std::move(task));
            }
            else
            {
                m_tasks.push(std::move(task));
            }
        }

        m_new_task_waiter.notify_all();
    }

    void TasksQueue::join()
    {
        auto lock = std::unique_lock(m_tasks_addition_mutex);

        m_is_joined = true;

        m_join_waiter.wait(lock, [this]() { return m_tasks.empty()
                                                    || m_terminate.load(std::memory_order_acquire); });

        if (!m_tasks.empty())
        {
            assert(m_terminate.load(std::memory_order_acquire));
            return;
        }

        m_tasks = std::exchange(m_tasks_after_join, {});
    }

    void TasksQueue::process_tasks()
    {
        while (!m_terminate.load(std::memory_order_acquire))
        {
            bool notify_joined_thread = false;

            {
                auto lock = std::unique_lock(m_tasks_addition_mutex);

                m_new_task_waiter.wait(lock, [this]() { return !m_tasks.empty()
                                                            || m_terminate.load(std::memory_order_acquire); });

                if (m_tasks.empty())
                {
                    assert(m_terminate.load(std::memory_order_acquire));
                    break;
                }

                auto &&task = m_tasks.front();

                lock.unlock();

                task();

                lock.lock();

                m_tasks.pop();
                
                notify_joined_thread = m_tasks.empty();
            }

            if (notify_joined_thread)
            {
                m_join_waiter.notify_all();
            }
        }
    }
}