create table if not exists users
(
    id           serial primary key,
    username     varchar(128) not null unique,
    password     varchar(512) not null,
    display_name varchar(128) not null,
    created_at   timestamptz default current_timestamp
);

create table if not exists sessions
(
    id         serial primary key,
    user_id    int references users (id) on delete cascade,
    token      varchar(128) not null,
    created_at timestamptz default current_timestamp
);

create table if not exists chats
(
    id          serial primary key,
    name        varchar(128),
    description varchar(256) null,
    type        smallint, -- 1 личный 2 группа
    created_at  timestamptz default current_timestamp
);

create table if not exists chat_members
(
    chat_id    int references chats (id) on delete cascade,
    user_id    int references users (id) on delete cascade,
    role       smallint    DEFAULT 0, -- 0 обычный, 1 админ
    created_at timestamptz default current_timestamp,
    primary key (chat_id, user_id)
);

create table if not exists messages
(
    id         serial primary key,
    chat_id    int           not null references chats (id) on delete cascade,
    user_id    int           null references users (id) on delete no action,
    content    varchar(2048) not null,
    edited_at  timestamptz   null,
    created_at timestamptz default current_timestamp
);

CREATE INDEX IF NOT EXISTS idx_messages_chat_id ON messages (chat_id);
CREATE INDEX IF NOT EXISTS idx_messages_user_id ON messages (user_id);
CREATE INDEX IF NOT EXISTS idx_sessions_user_id ON sessions (user_id);
CREATE INDEX IF NOT EXISTS idx_sessions_token ON sessions (token);