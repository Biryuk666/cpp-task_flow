#pragma once

#include <memory>
#include <thread>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace TaskFlow::infra {
    class ThreadPool {
    public:
        explicit ThreadPool(std::size_t thread_count);
        ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        [[nodiscard]] boost::asio::io_context& IOContext() noexcept;

        void Run();
        void Stop();

    private:
        std::size_t _thread_count;
        boost::asio::io_context _io_context;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work_guard;
        std::vector<std::jthread> _workers;
    };

}  // namespace TaskFlow::infra
