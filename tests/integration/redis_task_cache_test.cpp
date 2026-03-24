#include <gtest/gtest.h>
#include <sw/redis++/redis++.h>

#include "infra/redis_settings.h"
#include "infra/redis_task_cache.h"

using namespace TaskFlow;

namespace {
    infra::RedisSettings MakeSettings() {
        return {
            .host = "127.0.0.1",
            .port = 6379,
        };
    }

    void CleanUp(const infra::RedisSettings& settings, std::int64_t id) {
        sw::redis::Redis redis{settings.ToConnectionString()};
        redis.del("task:" + std::to_string(id));
    }

}  // namespace

TEST(RedisTaskCacheTest, PutAndGetTask) {
    const auto settings = MakeSettings();
    CleanUp(settings, 777);

    infra::RedisTaskCache cache{settings};

    const domain::Task task{777, "Cache task in Redis"};
    cache.Put(task);

    const auto loaded = cache.Get(777);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->ID(), 777);
    EXPECT_EQ(loaded->Title(), "Cache task in Redis");
    EXPECT_EQ(loaded->Status(), domain::TaskStatus::Created);
}

TEST(RedisTaskCacheTest, EraseRemovesTaskFromCache) {
    const auto settings = MakeSettings();
    CleanUp(settings, 778);

    infra::RedisTaskCache cache{settings};

    const domain::Task task{778, "Tassk to erase from Redis"};
    cache.Put(task);
    cache.Erase(778);

    const auto loaded = cache.Get(778);
    EXPECT_FALSE(loaded.has_value());
}
