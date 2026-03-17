add_executable(TaskFlow_tests
    unit/task_test.cpp
    unit/create_task_test.cpp
)

target_link_libraries(TaskFlow_tests
    PRIVATE
    TaskFlow_domain
    TaskFlow_infra
    TaskFlow_application
    GTest::gtest_main
)

target_compile_features(TaskFlow_tests PRIVATE cxx_std_23)

include(GoogleTest)
gtest_discover_tests(TaskFlow_tests)
