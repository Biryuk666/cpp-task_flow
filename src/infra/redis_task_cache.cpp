#include "redis_task_cache.h"

#include <stdexcept>

#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

namespace TaskFlow::infra {
    RedisTaskCache::RedisTaskCache(const RedisSettings& settings) : _settings(settings) {}

    void RedisTaskCache::Put(const domain::Task& task) {
        sw::redis::Redis redis{_settings.ToConnectionString()};

        // clang-format off
        nlohmann::json payload{
            {"id", task.ID()},
            {"title", task.Title()},
            {"status", StatusToString(task.Status())}
        };
        // clang-format on

        redis.set(MakeKey(task.ID()), payload.dump());
    }

    std::optional<domain::Task> RedisTaskCache::Get(std::int64_t id) const {
        sw::redis::Redis redis{_settings.ToConnectionString()};

        const auto value = redis.get(MakeKey(id));

        if (!value) {
            return std::nullopt;
        }

        const auto payload = nlohmann::json::parse(*value);

        // clang-format off
        return domain::Task{
            payload.at("id").get<std::int64_t>(),
            payload.at("title").get<std::string>(),
            StatusFromString(payload.at("status").get<std::string>())
        };
        // clang-format on
    }

    void RedisTaskCache::Erase(std::int64_t id) {
        sw::redis::Redis redis{_settings.ToConnectionString()};
        redis.del(MakeKey(id));
    }

    std::string RedisTaskCache::MakeKey(std::int64_t id) { return "task:" + std::to_string(id); }

    std::string RedisTaskCache::StatusToString(domain::TaskStatus status) { return domain::ToString(status); }

    domain::TaskStatus RedisTaskCache::StatusFromString(const std::string& value) {
        if (value == "Created") {
            return domain::TaskStatus::Created;
        } else if (value == "InProgress") {
            return domain::TaskStatus::InProgress;
        } else if (value == "Done") {
            return domain::TaskStatus::Done;
        } else if (value == "Canceled") {
            return domain::TaskStatus::Canceled;
        }

        throw std::runtime_error("Unknown task status from Redis: " + value);
    }

}  // namespace TaskFlow::infra
