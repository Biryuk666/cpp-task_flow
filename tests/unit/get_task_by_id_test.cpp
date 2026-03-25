#include <gtest/gtest.h>

#include "application/create_task.h"
#include "application/get_task_by_id.h"
#include "infra/memory_task_repository.h"

using namespace TaskFlow;

TEST(GetTaskByIDTest, ReturnsTaskWhenItExists) {
    infra::MemoryTaskRepository repository;
    application::CreateTask create_task{repository};
    application::GetTaskByID get_task_by_id{repository};

    auto dumb = create_task.Execute(501, "Read task by id");

    const auto task = get_task_by_id.Execute(501);

    ASSERT_TRUE(task.has_value());
    EXPECT_EQ(task->ID(), 501);
    EXPECT_EQ(task->Title(), "Read task by id");
}

TEST(GetTaskByIDTest, ReturnsEmptyWhenTaskDoesNotExist) {
    infra::MemoryTaskRepository repository;
    application::GetTaskByID get_task_by_id{repository};

    const auto task = get_task_by_id.Execute(9999);

    EXPECT_FALSE(task.has_value());
}
