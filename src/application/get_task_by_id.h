#pragma once

#include <optional>

#include "domain/task.h"
#include "infra/i_task_repository.h"

namespace TaskFlow::application {
    class GetTaskByID {
    public:
        explicit GetTaskByID(infra::ITaskRepository& repository);

        [[nodiscard]] std::optional<domain::Task> Execute(std::int64_t id) const;

    private:
        infra::ITaskRepository& _repository;
    };

}  // namespace TaskFlow::application
