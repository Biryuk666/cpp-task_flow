#pragma once

#include <unordered_map>

#include "i_task_repository.h"

namespace TaskFlow::infra {
    class MemoryTaskRepository final : public ITaskRepository {
    public:
        void Save(const domain::Task& task) override;
        [[nodiscard]] std::optional<domain::Task> FindByID(std::int64_t id) const override;

    private:
        std::unordered_map<std::int64_t, domain::Task> _storage;
    };

};  // namespace TaskFlow::infra
