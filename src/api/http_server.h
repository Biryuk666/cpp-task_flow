#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "application/create_task.h"
#include "application/get_task_by_id.h"

namespace TaskFlow::api {
    class HttpServer {
    public:
        HttpServer(boost::asio::io_context& io_context, const std::string& address, std::uint16_t port,
                   application::CreateTask& create_task, application::GetTaskByID& get_task_by_id);

        void Run();

    private:
        void DoAccept();
        void SetupSignalHandlers();

        boost::asio::io_context& _io_context;

        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::signal_set _signals;
        std::string _address;
        std::uint16_t _port;
        application::CreateTask& _create_task;
        application::GetTaskByID& _get_task_by_id;
    };

}  // namespace TaskFlow::api
