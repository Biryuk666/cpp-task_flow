#include "cached_task_repository.h"

namespace TaskFlow::infra {
    CachedTaskRepository::CachedTaskRepository(PostgresTaskRepository& primary_repository, RedisTaskCache& cache)
        : _primary_repository(primary_repository), _cache(cache) {}

    void CachedTaskRepository::Save(const domain::Task& task) {
        _primary_repository.Save(task);
        _cache.Put(task);
    }

    std::optional<domain::Task> CachedTaskRepository::FindByID(std::int64_t id) const {
        if (const auto cached = _cache.Get(id); cached.has_value()) {
            return cached;
        }

        const auto from_db = _primary_repository.FindByID(id);
        if (from_db.has_value()) {
            _cache.Put(from_db.value());
        }

        return from_db;
    }

}  // namespace TaskFlow::infra
