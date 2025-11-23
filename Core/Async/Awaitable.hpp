#pragma once

#include <future>
#include <chrono>
#include <coroutine>

namespace Core
{
    template <typename T>
    struct FutureAwaiter
    {
        std::future<T>& Future;

        bool await_ready() const
        {
            using namespace std::chrono_literals;
            return Future.wait_for(0ms) == std::future_status::ready;
        }

        void await_suspend(std::coroutine_handle<> Handle)
        {
            std::thread([this, Handle]() mutable
            {
                Future.wait();
                Handle.resume();
            }).detach();
        }

        T await_resume() const
        {
            return Future.get();
        }
    };

    template <>
    struct FutureAwaiter<void>
    {
        std::future<void>& Future;

        bool await_ready() const
        {
            using namespace std::chrono_literals;
            return Future.wait_for(0ms) == std::future_status::ready;
        }

        void await_suspend(std::coroutine_handle<> Handle)
        {
            std::thread([this, Handle]() mutable
            {
                Future.wait();
                Handle.resume();
            }).detach();
        }

        void await_resume()
        {
            Future.get();
        }
    };

    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>& Future)
    {
        return FutureAwaiter<T>{Future};
    }
}
