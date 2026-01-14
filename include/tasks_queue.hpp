#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>

namespace real_mouse::details
{
    class TasksQueue
    {
    public:
        using task_type = std::function<void()>;

    public:
        TasksQueue();

        TasksQueue(const TasksQueue&) = delete;
        TasksQueue(TasksQueue&&) = delete;

        TasksQueue& operator = (const TasksQueue&) = delete;
        TasksQueue& operator = (TasksQueue&&) = delete;

        ~TasksQueue();

    public:
        void add_task(task_type task);
        void join();

    private:
        void process_tasks();

    private:
        std::thread                     m_processor;
        std::queue<task_type>           m_tasks;
        std::queue<task_type>           m_tasks_after_join;
        std::atomic_bool                m_terminate;
        bool                            m_is_joined;

        mutable std::mutex              m_tasks_addition_mutex;
        mutable std::condition_variable m_new_task_waiter;
        mutable std::condition_variable m_join_waiter;
    };
}