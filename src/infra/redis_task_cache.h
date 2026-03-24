#pragma once

#include <optional>
#include <string>

#include "domain/task.h"
#include "infra/redis_settings.h"

namespace TaskFlow::infra {
    class RedisTaskCache {
    public:
        explicit RedisTaskCache(const RedisSettings& settings);

        void Put(const domain::Task& task);
        [[nodiscard]] std::optional<domain::Task> Get(std::int64_t id) const;
        void Erase(std::int64_t id);

    private:
        [[nodiscard]] static std::string MakeKey(std::int64_t id);
        [[nodiscard]] static std::string StatusToString(domain::TaskStatus status);
        [[nodiscard]] static domain::TaskStatus StatusFromString(const std::string& value);

        RedisSettings _settings;
    };

}  // namespace TaskFlow::infra
