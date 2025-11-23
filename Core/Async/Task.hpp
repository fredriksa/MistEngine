#pragma once

#include <coroutine>
#include <exception>

namespace Core
{
    class Task
    {
    public:
        struct promise_type
        {
            std::exception_ptr exception = nullptr;

            Task get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            std::suspend_never initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            void return_void() noexcept
            {
            }

            void unhandled_exception()
            {
                exception = std::current_exception();
            }
        };

        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        Task(Task&& Other) noexcept : Handle(Other.Handle)
        {
            Other.Handle = nullptr;
        }

        Task& operator=(Task&& Other) noexcept
        {
            if (this != &Other)
            {
                if (Handle) Handle.destroy();
                Handle = Other.Handle;
                Other.Handle = nullptr;
            }
            return *this;
        }

        ~Task()
        {
            if (Handle)
            {
                Handle.destroy();
            }
        }

        bool await_ready() const noexcept
        {
            return Handle.done();
        }

        void await_suspend(std::coroutine_handle<promise_type> continuation) noexcept
        {
        }

        void await_resume() const
        {
            if (Handle.promise().exception)
            {
                std::rethrow_exception(Handle.promise().exception);
            }
        }

    private:
        std::coroutine_handle<promise_type> Handle;

        explicit Task(std::coroutine_handle<promise_type> Handle) : Handle(Handle)
        {
        }
    };
}
