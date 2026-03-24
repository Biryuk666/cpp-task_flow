#pragma once

#include <cstdint>
#include <string>

namespace TaskFlow::infra {
    struct RedisSettings {
        std::string host;
        std::uint16_t port;

        [[nodiscard]] std::string ToConnectionString() const { return "tcp://" + host + ":" + std::to_string(port); }
    };

}  // namespace TaskFlow::infra
