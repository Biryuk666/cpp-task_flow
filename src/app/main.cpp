
#include <spdlog/spdlog.h>

#include "api/http_server.h"
#include "application/create_task.h"
#include "application/get_task_by_id.h"
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

    api::HttpServer server{"0.0.0.0", 8080, create_task, get_task_by_id};

    spdlog::info("starting TaskFlow API");
    server.Run();

    return 0;
}
