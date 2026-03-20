#include <gtest/gtest.h>
#include <pqxx/pqxx>

#include "infra/postgres_settings.h"
#include "infra/postgres_task_repository.h"

using namespace TaskFlow;

namespace {
    infra::PostgresSettings MakeSettings() {
        return {.host = "127.0.0.1",
                .port = 5432,
                .db_name = "TaskFlow_db",
                .user = "TaskFlow_user",
                .password = "TaskFlow_password"};
    }

    void TruncateTasks(const infra::PostgresSettings& settings) {
        pqxx::connection connection{settings.ToConnectionString()};
        pqxx::work tx{connection};
        tx.exec("TRUNCATE TABLE tasks");
        tx.commit();
    }

}  // namespace

TEST(PostgresTaskRepositoryTest, SaveAndFindByID) {
    const auto settings = MakeSettings();
    TruncateTasks(settings);

    infra::PostgresTaskRepository repository{settings};

    const domain::Task task{101, "Persist task in PostgreSQL"};
    repository.Save(task);

    const auto loaded = repository.FindByID(101);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->ID(), 101);
    EXPECT_EQ(loaded->Title(), "Persist task in PostgreSQL");
    EXPECT_EQ(loaded->Status(), domain::TaskStatus::Created);
}
