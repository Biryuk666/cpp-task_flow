#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "application/create_task.h"
#include "domain/task.h"
#include "infra/memory_task_repository.h"

using namespace TaskFlow;

int main() {
    infra::MemoryTaskRepository repository;
    application::CreateTask create_task(repository);

    const auto task = create_task.Execute(1, "Learn backend with C++");

    // clang-format off
    nlohmann::json payload{
        {"id", task.ID()},
        {"title", task.Title()},
        {"status", domain::ToString(task.Status())}
    };
    // clang-format on

    spdlog::info("Task created : {}", payload.dump());
}
