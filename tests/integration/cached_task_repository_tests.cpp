#include <gtest/gtest.h>
#include <pqxx/pqxx>
#include <sw/redis++/redis++.h>

#include "infra/cached_task_repository.h"
#include "infra/postgres_settings.h"
#include "infra/postgres_task_repository.h"
#include "infra/redis_settings.h"
#include "infra/redis_task_cache.h"

using namespace TaskFlow;

namespace {
    infra::PostgresSettings MakePostgresSettings() {
        return {
            .host = "127.0.0.1",
            .port = 5432,
            .db_name = "TaskFlow_db",
            .user = "TaskFlow_user",
            .password = "TaskFlow_password",
        };
    }

    infra::RedisSettings MakeRedisSettings() {
        return {
            .host = "127.0.0.1",
            .port = 6379,
        };
    }

    void TruncateTask(const infra::PostgresSettings& settings) {
        pqxx::connection connection{settings.ToConnectionString()};
        pqxx::work tx{connection};
        tx.exec("TRUNCATE TABLE tasks");
        tx.commit();
    }

    void DeleteCacheKey(const infra::RedisSettings& settings, std::int64_t id) {
        sw::redis::Redis redis{settings.ToConnectionString()};
        redis.del("task:" + std::to_string(id));
    }

}  // namespace

TEST(CachedTaskRepositoryTest, FindByIDLoadsFromPostgresAndWarmRedisCache) {
    const auto postgres_settings = MakePostgresSettings();
    const auto redis_settings = MakeRedisSettings();

    TruncateTask(postgres_settings);
    DeleteCacheKey(redis_settings, 902);

    infra::PostgresTaskRepository postgres_repository{postgres_settings};
    infra::RedisTaskCache redis_cache{redis_settings};
    infra::CachedTaskRepository repository{postgres_repository, redis_cache};

    const domain::Task task{902, "Warm cache from db"};
    postgres_repository.Save(task);

    const auto before_cache = redis_cache.Get(902);
    EXPECT_FALSE(before_cache.has_value());

    auto loaded = repository.FindByID(902);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->Title(), "Warm cache from db");

    const auto after_cache = redis_cache.Get(902);
    ASSERT_TRUE(after_cache.has_value());
    EXPECT_EQ(after_cache->Title(), "Warm cache from db");
}
