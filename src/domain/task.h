#pragma once

#include <cstdint>
#include <string>

namespace TaskFlow::domain {
    enum class TaskStatus { Created, InProgress, Done, Canceled };

    class Task {
    public:
        Task(std::int64_t id, const std::string& title, TaskStatus status = TaskStatus::Created);

        [[nodiscard]] std::int64_t ID() const noexcept;
        [[nodiscard]] const std::string& Title() const noexcept;
        [[nodiscard]] TaskStatus Status() const noexcept;

        void Start();
        void Complete();
        void Cancel();

    private:
        std::int64_t _id;
        std::string _title;
        TaskStatus _status;
    };

    std::string ToString(TaskStatus status);

}  // namespace TaskFlow::domain
