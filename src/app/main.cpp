#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <string>

int main() {
    nlohmann::json payload {
        {"service", "TaskFlow"},
        {"version", "0.1.0"},
        {"status", "starting"}
    };

    spdlog::info("TaskFlow app started");
    spdlog::info("Payload: {}", payload.dump());
}
