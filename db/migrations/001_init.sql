CREATE TABLE
    IF NOT EXISTS tasks (
        id BIGINT PRIMARY KEY,
        title TEXT NOT NULL,
        status TEXT NOT NULL,
        created_at TIMESTAMPTZ NOT NULL DEFAULT NOW ()
    );

CREATE INDEX IF NOT EXISTS idx_task_status ON tasks (status);

CREATE INDEX IF NOT EXISTS idx_tasks_created_at ON tasks (created_at);
