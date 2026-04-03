#pragma once

#include <cstdint>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "application/create_task.h"
#include "application/get_task_by_id.h"

namespace TaskFlow::api {
    class HttpServer {
    public:
        HttpServer(const std::string& address, std::uint16_t port, application::CreateTask& create_task,
                   application::GetTaskByID& get_task_by_id);

        void Run();

    private:
        std::string _address;
        std::uint16_t _port;
        application::CreateTask& _create_task;
        application::GetTaskByID& _get_task_by_id;
    };

}  // namespace TaskFlow::api
