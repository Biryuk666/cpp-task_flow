#include "http_server.h"

#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
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
                               application::GetTaskByID& get_task_by_id) {
            const std::string_view target(request.target().data(), request.target().size());

            spdlog::info("Incominng request: method={}, target={}", std::string(http::to_string(request.method())),
                         target);

            if (request.method() == http::verb::get && target == "/health") {
                return MakeJsonResponse(http::status::ok, request.version(), {{"status", "ok"}});
            } else if (request.method() == http::verb::post && target == "/tasks") {
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

        class HttpSession : public std::enable_shared_from_this<HttpSession> {
        public:
            HttpSession(tcp::socket socket, application::CreateTask& create_task,
                        application::GetTaskByID& get_task_by_id)
                : _socket(std::move(socket)), _create_task(create_task), _get_task_by_id(get_task_by_id) {}

            void Run() { DoRead(); }

        private:
            void DoRead() {
                _request = {};
                http::async_read(_socket, _buffer, _request,
                                 beast::bind_front_handler(&HttpSession::OnRead, shared_from_this()));
            }

            void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                if (ec == http::error::end_of_stream) {
                    DoClose();
                    return;
                } else if (ec) {
                    spdlog::error("HTTP read failed: {}", ec.message());
                    return;
                }

                _response = HandleRequest(_request, _create_task, _get_task_by_id);

                http::async_write(_socket, _response,
                                  beast::bind_front_handler(&HttpSession::OnWrite, shared_from_this()));
            }

            void OnWrite(beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                if (ec) {
                    spdlog::error("HTTP write failed: {}", ec.message());
                    return;
                }

                DoClose();
            }

            void DoClose() {
                beast::error_code ec;
                _socket.shutdown(tcp::socket::shutdown_send, ec);
            }

            tcp::socket _socket;
            beast::flat_buffer _buffer;
            Request _request;
            Response _response;
            application::CreateTask& _create_task;
            application::GetTaskByID& _get_task_by_id;
        };

    }  // namespace

    HttpServer::HttpServer(boost::asio::io_context& io_context, const std::string& address, std::uint16_t port,
                           application::CreateTask& create_task, application::GetTaskByID& get_task_by_id)
        : _io_context(io_context),
          _acceptor(io_context),
          _signals(io_context, SIGINT, SIGTERM),
          _address(address),
          _port(port),
          _create_task(create_task),
          _get_task_by_id(get_task_by_id) {
        const auto ip_address = net::ip::make_address(_address);

        tcp::endpoint endpoint{ip_address, _port};

        _acceptor.open(endpoint.protocol());
        _acceptor.set_option(net::socket_base::reuse_address(true));
        _acceptor.bind(endpoint);
        _acceptor.listen(net::socket_base::max_listen_connections);
    }

    void HttpServer::Run() {
        spdlog::info("HTTP srver listening on {}:{}", _address, _port);
        SetupSignalHandlers();
        DoAccept();
    }

    void HttpServer::DoAccept() {
        _acceptor.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (ec) {
                if (ec == net::error::operation_aborted) {
                    return;
                }

                spdlog::error("Accept failed: {}", ec.message());
            } else {
                auto session = std::make_shared<HttpSession>(std::move(socket), _create_task, _get_task_by_id);
                session->Run();
            }

            if (_acceptor.is_open()) {
                DoAccept();
            }
        });
    }

    void HttpServer::SetupSignalHandlers() {
        _signals.async_wait([this](const beast::error_code& ec, int signal_number) {
            if (ec) {
                spdlog::error("Signal handler failed: {}", ec.message());
                return;
            }

            spdlog::info("Reciever signal {}, stopping server", signal_number);

            beast::error_code close_ec;
            _acceptor.close(close_ec);
            _io_context.stop();
        });
    }

}  // namespace TaskFlow::api
