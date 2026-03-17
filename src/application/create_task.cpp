#include "create_task.h"

namespace TaskFlow::application {
    CreateTask::CreateTask(infra::ITaskRepository& repository) : _repository(repository) {}

    domain::Task CreateTask::Execute(std::int64_t id, const std::string& title) const {
        domain::Task task{id, title};
        _repository.Save(task);
        return task;
    }

}  // namespace TaskFlow::application
