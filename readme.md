# MessangerRestApi

REST API мессенджер на C++ с использованием Crow framework и PostgreSQL.

## Возможности

- 🔐 Аутентификация пользователей (регистрация, логин, logout)
- 👥 Управление пользователями 
- 💬 Личные и групповые чаты
- 📝 Отправка, редактирование и удаление сообщений
- 🛡️ Middleware для авторизации
- 📊 Логирование с помощью spdlog

## Технологии

- **C++20**
- **Crow** - веб-фреймворк
- **SOCI** - библиотека для работы с БД
- **PostgreSQL** - база данных
- **spdlog** - логирование
- **CMake** - система сборки

## Архитектура

Проект использует многоуровневую архитектуру:

```
┌─────────────┐
│  Handlers   │ ← HTTP endpoints
├─────────────┤
│  Services   │ ← Business logic
├─────────────┤
│ Repository  │ ← Data access
├─────────────┤
│  Database   │ ← PostgreSQL
└─────────────┘
```

## Установка и запуск

### Требования

- CMake 3.29+
- C++20 совместимый компилятор
- PostgreSQL
- vcpkg для управления пакетами

### Зависимости

```bash
# Установка зависимостей через vcpkg
vcpkg install soci[postgresql]:x64-mingw-dynamic
vcpkg install spdlog:x64-mingw-dynamic
vcpkg install crow:x64-mingw-dynamic
```

### Настройка базы данных

1. Создайте базу данных PostgreSQL
2. Настройте строку подключения в `main.cpp`:

```cpp
soci::session sql("postgresql", "dbname=(dbname) user=(postgres) password=(password) host=(localhost)");
```

### Сборка

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

### Запуск

```bash
./MessagerV1
```

Сервер запустится на порту 8080.

## API Endpoints

### Аутентификация
- `POST /register` - Регистрация пользователя
- `POST /login` - Вход в систему  
- `DELETE /logout` - Выход
- `DELETE /logout-all` - Выход со всех устройств

### Пользователи
- `GET /users/<id>` - Получить пользователя по ID
- `GET /users/<username>` - Получить пользователя по username
- `PATCH /me` - Обновить профиль
- `POST /me/change-password` - Изменить пароль
- `DELETE /me` - Удалить аккаунт

### Чаты
- `GET /chats/<id>` - Получить чат
- `GET /chats/<id>/users` - Получить участников чата
- `GET /me/chats` - Получить мои чаты
- `POST /chats/personal` - Создать личный чат
- `POST /chats/group` - Создать групповой чат
- `PATCH /chats/<id>` - Обновить групповой чат
- `POST /chats/<id>/users/<user_id>` - Добавить пользователя в группу
- `DELETE /chats/<id>/users/<user_id>` - Удалить из группы
- `PATCH /chats/<id>/users/<user_id>/promote` - Повысить до админа
- `DELETE /chats/<id>` - Удалить чат

### Сообщения
- `GET /messages/<id>` - Получить сообщение
- `GET /chats/<id>/messages` - Получить сообщения чата
- `GET /chats/<id>/messages/after/<message_id>` - Сообщения после ID
- `GET /chats/<id>/messages/before/<message_id>` - Сообщения до ID  
- `GET /chats/<id>/messages/last` - Последнее сообщение
- `POST /chats/<id>/messages` - Отправить сообщение
- `PATCH /messages/<id>` - Редактировать сообщение
- `DELETE /messages/<id>` - Удалить сообщение

## Структура проекта

```
src/
├── domain/          # Доменные модели
├── repository/      # Слой данных
├── service/         # Бизнес-логика  
├── handler/         # HTTP handlers
├── middleware/      # Middleware компоненты
├── context/         # Контексты
├── helpers/         # Утилиты
├── logger/          # Логирование
└── exceptions/      # Исключения
```

## Лицензия

MIT License