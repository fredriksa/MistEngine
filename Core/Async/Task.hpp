#pragma once

#include <coroutine>
#include <exception>
#include <optional>

namespace Core
{
    template <typename T = void>
    class Task
    {
    public:
        struct promise_type
        {
            std::exception_ptr exception = nullptr;
            std::optional<T> value;
            std::coroutine_handle<> continuation;

            Task get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            std::suspend_never initial_suspend() noexcept { return {}; }

            auto final_suspend() noexcept
            {
                struct FinalAwaiter
                {
                    bool await_ready() noexcept { return false; }

                    void await_suspend(std::coroutine_handle<promise_type> h) noexcept
                    {
                        if (h.promise().continuation)
                            h.promise().continuation.resume();
                    }

                    void await_resume() noexcept {}
                };
                return FinalAwaiter{};
            }

            void return_value(T val) noexcept
            {
                value = std::move(val);
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

        void await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            Handle.promise().continuation = continuation;
        }

        T await_resume()
        {
            if (Handle.promise().exception)
            {
                std::rethrow_exception(Handle.promise().exception);
            }
            return std::move(*Handle.promise().value);
        }

    private:
        std::coroutine_handle<promise_type> Handle;

        explicit Task(std::coroutine_handle<promise_type> Handle) : Handle(Handle)
        {
        }
    };

    template <>
    class Task<void>
    {
    public:
        struct promise_type
        {
            std::exception_ptr exception = nullptr;
            std::coroutine_handle<> continuation;

            Task get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            std::suspend_never initial_suspend() noexcept { return {}; }

            auto final_suspend() noexcept
            {
                struct FinalAwaiter
                {
                    bool await_ready() noexcept { return false; }

                    void await_suspend(std::coroutine_handle<promise_type> h) noexcept
                    {
                        if (h.promise().continuation)
                            h.promise().continuation.resume();
                    }

                    void await_resume() noexcept {}
                };
                return FinalAwaiter{};
            }

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

        void await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            Handle.promise().continuation = continuation;
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
