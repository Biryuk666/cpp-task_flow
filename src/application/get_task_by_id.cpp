#include "get_task_by_id.h"

namespace TaskFlow::application {
    GetTaskByID::GetTaskByID(infra::ITaskRepository& repository) : _repository(repository) {}

    std::optional<domain::Task> GetTaskByID::Execute(std::int64_t id) const { return _repository.FindByID(id); }

}  // namespace TaskFlow::application
