#include "thread_pool.h"

#include <spdlog/spdlog.h>

namespace TaskFlow::infra {
    ThreadPool::ThreadPool(std::size_t thread_count)
        : _thread_count(thread_count == 0 ? 1 : thread_count),
          _io_context(static_cast<int>(_thread_count)),
          _work_guard(boost::asio::make_work_guard(_io_context)) {}

    ThreadPool::~ThreadPool() { Stop(); }

    boost::asio::io_context& ThreadPool::IOContext() noexcept { return _io_context; }

    void ThreadPool::Run() {
        _workers.reserve(_thread_count);

        for (std::size_t i = 0; i < _thread_count; ++i) {
            _workers.emplace_back([this, i]() {
                try {
                    spdlog::info("Worker thread {} started", i);
                    _io_context.run();
                    spdlog::info("Worker thread {}  stopped", i);
                } catch (const std::exception& ex) {
                    spdlog::error("Wrker thread {} failed: {}", i, ex.what());
                }
            });
        }
    }

    void ThreadPool::Stop() {
        _work_guard.reset();
        _io_context.stop();
    }

}  // namespace TaskFlow::infra
