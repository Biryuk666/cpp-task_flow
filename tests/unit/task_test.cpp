#include <gtest/gtest.h>

#include "domain/task.h"

using namespace TaskFlow;

TEST(TaskTest, NewTaskHasCreatedStatus) {
    const domain::Task& task{1, "Test task"};
    EXPECT_EQ(task.Status(), domain::TaskStatus::Created);
}

TEST(TaskTest, TaskCanBeStarted) {
    domain::Task task{1, "Test task"};
    task.Start();
    EXPECT_EQ(task.Status(), domain::TaskStatus::InProgress);
}

TEST(TaskTest, TaskCanBeCompletedOnlyAfterStart) {
    domain::Task task{1, "Test task"};
    EXPECT_THROW(task.Complete(), std::logic_error);
}
