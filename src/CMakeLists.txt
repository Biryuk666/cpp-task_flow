add_executable(TaskFlow_app app/main.cpp)

target_link_libraries(TaskFlow_app
    PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

target_compile_features(TaskFlow_app PRIVATE cxx_std_23)
