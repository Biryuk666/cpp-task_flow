#include "postgres_task_repository.h"

#include <stdexcept>

namespace TaskFlow::infra {
    PostgresTaskRepository::PostgresTaskRepository(const PostgresSettings& settings) : _settings(settings) {}

    void PostgresTaskRepository::Save(const domain::Task& task) {
        pqxx::connection connection{_settings.ToConnectionString()};
        pqxx::work tx{connection};

        tx.exec_params(
            R"(INSERT INTO tasks (id, title, status)
            VALUES ($1, $2, $3)
            ON CONFLICT (id) DO UPDATE
            SET title = EXCLUDED.title,
                status = EXCLUDED.status)",
            task.ID(), task.Title(), StatusToDB(task.Status()));

        tx.commit();
    }

    std::optional<domain::Task> PostgresTaskRepository::FindByID(std::int64_t id) const {
        pqxx::connection connection{_settings.ToConnectionString()};
        pqxx::read_transaction tx{connection};

        const pqxx::result result = tx.exec_params(
            R"(
            SELECT id, title, status
            FROM tasks
            WHERE id = $1
            )",
            id);

        if (result.empty()) {
            return std::nullopt;
        }

        const pqxx::row row = result.front();

        const auto task_id = row["id"].as<std::int64_t>();
        const auto title = row["title"].as<std::string>();
        const auto status = StatusFromDB(row["status"].as<std::string>());

        return domain::Task{task_id, title, status};
    }

    std::string PostgresTaskRepository::StatusToDB(domain::TaskStatus status) { return domain::ToString(status); }

    domain::TaskStatus PostgresTaskRepository::StatusFromDB(const std::string& value) {
        if (value == "Created") {
            return domain::TaskStatus::Created;
        } else if (value == "InProgress") {
            return domain::TaskStatus::InProgress;
        } else if (value == "Done") {
            return domain::TaskStatus::Done;
        } else if (value == "Canceled") {
            return domain::TaskStatus::Cancelled;
        }

        throw std::runtime_error("Unknown task status from database: " + value);
    }

}  // namespace TaskFlow::infra
