#include <memory>

#include <gtest/gtest.h>

#include "application/create_task.h"
#include "infra/memory_task_repository.h"

using namespace TaskFlow;

TEST(CreateTaskTest, ExecuteCreatesAndStoresTask) {
    infra::MemoryTaskRepository repository;
    application::CreateTask create_task(repository);

    const auto task = create_task.Execute(42, "Write integration test");

    EXPECT_EQ(task.ID(), 42);
    EXPECT_EQ(task.Title(), "Write integration test");

    const auto stored = repository.FindByID(42);
    ASSERT_TRUE(stored.has_value());
    EXPECT_EQ(stored->Title(), "Write integration test");
}
