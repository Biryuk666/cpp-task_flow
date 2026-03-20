#pragma once

#include <optional>
#include <string>

#include <pqxx/pqxx>

#include "infra/i_task_repository.h"
#include "infra/postgres_settings.h"

namespace TaskFlow::infra {
    class PostgresTaskRepository final : ITaskRepository {
    public:
        explicit PostgresTaskRepository(const PostgresSettings& settings);

        void Save(const domain::Task& task) override;
        [[nodiscard]] std::optional<domain::Task> FindByID(std::int64_t id) const override;

    private:
        [[nodiscard]] static std::string StatusToDB(domain::TaskStatus status);
        [[nodiscard]] static domain::TaskStatus StatusFromDB(const std::string& value);

        PostgresSettings _settings;
    };
}  // namespace TaskFlow::infra
