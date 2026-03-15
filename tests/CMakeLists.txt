add_executable(TaskFlow_tests unit/sample_test.cpp)

target_link_libraries(TaskFlow_tests
    PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TaskFlow_tests)
