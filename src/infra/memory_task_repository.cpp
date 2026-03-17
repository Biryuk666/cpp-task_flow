#include "memory_task_repository.h"

namespace TaskFlow::infra {
    void MemoryTaskRepository::Save(const domain::Task& task) { _storage.insert_or_assign(task.ID(), task); }

    std::optional<domain::Task> MemoryTaskRepository::FindByID(std::int64_t id) const {
        const auto it = _storage.find(id);

        if (it == _storage.end()) {
            return std::nullopt;
        }

        return it->second;
    }

}  // namespace TaskFlow::infra
