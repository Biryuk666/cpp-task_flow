#include "http_server.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "domain/task.h"

namespace TaskFlow::api {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    namespace {
        using Request = http::request<http::string_body>;
        using Response = http::response<http::string_body>;

        Response MakeJsonResponse(http::status status, unsigned version, const nlohmann::json& body) {
            Response response{status, version};
            response.set(http::field::content_type, "application/json");
            response.body() = body.dump() + "\n";
            response.prepare_payload();
            response.keep_alive(false);

            return response;
        }

        Response MakeTextResponse(http::status status, unsigned version, const std::string& body) {
            Response response(status, version);
            response.set(http::field::content_type, "text/plain; charset=utf-8");
            response.body() = body;
            response.prepare_payload();
            response.keep_alive(false);

            return response;
        }

        std::optional<std::int64_t> ParseTaskID(std::string_view target) {
            constexpr std::string_view prefix = "/tasks/";
            if (!target.starts_with(prefix)) {
                return std::nullopt;
            }

            const auto id_part = target.substr(prefix.size());
            if (id_part.empty()) {
                return std::nullopt;
            }

            std::size_t pos = 0;
            const auto id = std::stoll(std::string{id_part}, &pos);
            if (pos != id_part.size()) {
                return std::nullopt;
            }

            return static_cast<std::int64_t>(id);
        }

        nlohmann::json TaskToJson(const domain::Task& task) {
            // clang-format off
            return {
                {"id", task.ID()},
                {"title", task.Title()},
                {"status", domain::ToString(task.Status())}
            };
            // clang-format on
        }

        Response HandleRequest(const Request& request, application::CreateTask& create_task,
                               application::GetTaskByID get_task_by_id) {
            if (request.method() == http::verb::get && request.target() == "/health") {
                return MakeJsonResponse(http::status::ok, request.version(), {{"status", "ok"}});
            } else if (request.method() == http::verb::post && request.target() == "/tasks") {
                try {
                    const auto payload = nlohmann::json::parse(request.body());

                    const auto id = payload.at("id").get<std::int64_t>();
                    const auto title = payload.at("title").get<std::string>();

                    const auto task = create_task.Execute(id, title);

                    return MakeJsonResponse(http::status::created, request.version(), TaskToJson(task));
                } catch (const std::exception& ex) {
                    return MakeJsonResponse(http::status::bad_request, request.version(), {{"error", ex.what()}});
                }
            } else if (request.method() == http::verb::get) {
                if (const auto task_id = ParseTaskID(request.target())) {
                    const auto task = get_task_by_id.Execute(*task_id);

                    if (!task.has_value()) {
                        return MakeJsonResponse(http::status::not_found, request.version(),
                                                {{"error", "task not found"}});
                    }

                    return MakeJsonResponse(http::status::ok, request.version(), TaskToJson(*task));
                }
            }

            return MakeJsonResponse(http::status::not_found, request.version(), {{"error", "route not found"}});
        }

        void HandleSession(tcp::socket socket, application::CreateTask& create_task,
                           application::GetTaskByID& get_task_by_id) {
            beast::flat_buffer buffer;
            Request request;

            http::read(socket, buffer, request);

            const auto response = HandleRequest(request, create_task, get_task_by_id);
            http::write(socket, response);

            beast::error_code ec;
            socket.shutdown(tcp::socket::shutdown_send, ec);
        }

    }  // namespace

    HttpServer::HttpServer(const std::string& address, std::uint16_t port, application::CreateTask& create_task,
                           application::GetTaskByID& get_task_by_id)
        : _address(address), _port(port), _create_task(create_task), _get_task_by_id(get_task_by_id) {}

    void HttpServer::Run() {
        net::io_context ioc{1};

        const auto ip_address = net::ip::make_address(_address);
        tcp::acceptor acceptor{ioc, {ip_address, _port}};

        spdlog::info("HTTP server listening on {}:{}", _address, _port);

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            try {
                HandleSession(std::move(socket), _create_task, _get_task_by_id);

            } catch (const std::exception& ex) {
                spdlog::error("HTTP session failed: {}", ex.what());
            }
        }
    }

}  // namespace TaskFlow::api
