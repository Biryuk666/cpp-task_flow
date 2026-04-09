find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)
find_package(redis++ REQUIRED)
find_package(Boost REQUIRED)

add_library(TaskFlow_domain
    domain/task.cpp
)

target_include_directories(TaskFlow_domain
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(TaskFlow_domain PUBLIC cxx_std_23)

add_library(TaskFlow_infra
    infra/memory_task_repository.cpp
    infra/postgres_task_repository.cpp
    infra/redis_task_cache.cpp
    infra/cached_task_repository.cpp
    infra/thread_pool.cpp
)

target_include_directories(TaskFlow_infra
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TaskFlow_infra
    PUBLIC
    TaskFlow_domain
    libpqxx::pqxx
    redis++::redis++_static
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Boost::headers
)

target_compile_features(TaskFlow_infra PUBLIC cxx_std_23)

add_library(TaskFlow_application
    application/create_task.cpp
    application/get_task_by_id.cpp
)

target_include_directories(TaskFlow_application
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TaskFlow_application
    PUBLIC
    TaskFlow_domain
    TaskFlow_infra
)

target_compile_features(TaskFlow_application PUBLIC cxx_std_23)

add_library(TaskFlow_api
    api/http_server.cpp
)

target_include_directories(TaskFlow_api
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TaskFlow_api
    PUBLIC
    TaskFlow_application
    TaskFlow_domain
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Boost::headers
)

target_compile_features(TaskFlow_api PUBLIC cxx_std_23)

add_executable(TaskFlow_app app/main.cpp)

target_link_libraries(TaskFlow_app
    PRIVATE
    TaskFlow_domain
    TaskFlow_infra
    TaskFlow_application
    TaskFlow_api
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

target_compile_features(TaskFlow_app PRIVATE cxx_std_23)
