#pragma once

#include <cstdint>
#include <string>

namespace TaskFlow::infra {
    struct PostgresSettings {
        std::string host;
        std::uint16_t port;
        std::string db_name;
        std::string user;
        std::string password;

        [[nodiscard]] std::string ToConnectionString() const {
            return "host=" + host + " port=" + std::to_string(port) + " dbname=" + db_name + " user=" + user +
                   " password=" + password;
        }
    };

}  // namespace TaskFlow::infra
