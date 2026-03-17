#pragma once

#include <string>

#include "domain/task.h"
#include "infra/i_task_repository.h"

namespace TaskFlow::application {
    class CreateTask {
    public:
        explicit CreateTask(infra::ITaskRepository& repository);

        [[nodiscard]] domain::Task Execute(std::int64_t id, const std::string& title) const;

    private:
        infra::ITaskRepository& _repository;
    };

}  // namespace TaskFlow::application
