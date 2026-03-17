#include "task.h"

#include <stdexcept>

namespace TaskFlow::domain {
    Task::Task(std::int64_t id, const std::string& title, TaskStatus status)
        : _id(id), _title(title), _status(status) {}

    std::int64_t Task::ID() const noexcept { return _id; }

    const std::string& Task::Title() const noexcept { return _title; }

    TaskStatus Task::Status() const noexcept { return _status; }

    void Task::Start() {
        if (_status != TaskStatus::Created) {
            throw std::logic_error("Task can be started only from 'Created' state");
        }

        _status = TaskStatus::InProgress;
    }

    void Task::Complete() {
        if (_status != TaskStatus::InProgress) {
            throw std::logic_error("Task can be completed only from 'InProgress' state");
        }

        _status = TaskStatus::Done;
    }

    void Task::Cancel() {
        if (_status == TaskStatus::Done) {
            throw std::logic_error("Completed task cannot be cancelled");
        }

        if (_status == TaskStatus::Cancelled) {
            throw std::logic_error("Task is already canceled");
        }

        _status = TaskStatus::Cancelled;
    }

    std::string ToString(TaskStatus status) {
        switch (status) {
            case TaskStatus::Created:
                return "Created";
            case TaskStatus::InProgress:
                return "InProgress";
            case TaskStatus::Done:
                return "Done";
            case TaskStatus::Cancelled:
                return "Cancelled";
        }

        throw std::logic_error("Unknown TaskStatus");
    }

}  // namespace TaskFlow::domain
