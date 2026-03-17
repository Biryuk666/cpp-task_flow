#pragma once

#include <optional>

#include "domain/task.h"

namespace TaskFlow::infra {
    class ITaskRepository {
    public:
        virtual ~ITaskRepository() = default;

        virtual void Save(const domain::Task& task) = 0;
        [[nodiscard]] virtual std::optional<domain::Task> FindByID(std::int64_t id) const = 0;
    };

}  // namespace TaskFlow::infra
