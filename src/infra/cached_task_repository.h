#pragma once

#include "infra/i_task_repository.h"
#include "postgres_task_repository.h"
#include "infra/redis_task_cache.h"

namespace TaskFlow::infra {
    class CachedTaskRepository final : public ITaskRepository {
    public:
        CachedTaskRepository(PostgresTaskRepository& primary_repository, RedisTaskCache& cache);

        void Save(const domain::Task& task) override;
        [[nodiscard]] std::optional<domain::Task> FindByID(std::int64_t id) const override;

    private:
        PostgresTaskRepository& _primary_repository;
        RedisTaskCache& _cache;
    };

}  // namespace TaskFlow::infra
