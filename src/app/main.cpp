#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "application/create_task.h"
#include "application/get_task_by_id.h"
#include "domain/task.h"
#include "infra/cached_task_repository.h"
#include "infra/postgres_settings.h"
#include "infra/postgres_task_repository.h"
#include "infra/redis_settings.h"
#include "infra/redis_task_cache.h"

using namespace TaskFlow;

int main() {
    using namespace TaskFlow;

    infra::PostgresSettings postgres_settings{
        .host = "127.0.0.1",
        .port = 5432,
        .db_name = "TaskFlow_db",
        .user = "TaskFlow_user",
        .password = "TaskFlow_password",
    };

    infra::RedisSettings redis_settings{
        .host = "127.0.0.1",
        .port = 6379,
    };

    infra::PostgresTaskRepository postgres_repository{postgres_settings};
    infra::RedisTaskCache redis_cache{redis_settings};
    infra::CachedTaskRepository repository{postgres_repository, redis_cache};

    application::CreateTask create_task(repository);
    application::GetTaskByID get_task_by_id(repository);

    const auto dumb = create_task.Execute(1, "Learn cache-aside with PostgreSQL and Redis");
    const auto task = get_task_by_id.Execute(1);

    if (!task.has_value()) {
        spdlog::error("Task not found");

        return 1;
    }

    // clang-format off
    nlohmann::json payload{
        {"id", task->ID()},
        {"title", task->Title()},
        {"status", domain::ToString(task->Status())}
    };
    // clang-format on

    spdlog::info("Loaded task: {}", payload.dump());

    return 0;
}
