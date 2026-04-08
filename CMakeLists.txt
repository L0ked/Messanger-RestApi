cmake_minimum_required(VERSION 3.29)
project(MessagerV1)

set(CMAKE_CXX_STANDARD 20)

find_package(soci CONFIG REQUIRED COMPONENTS core postgresql)
find_package(spdlog CONFIG REQUIRED)

include_directories(src)

add_executable(MessagerV1 main.cpp
        src/domain/User.h
        src/repository/IUserRepository.h
        src/repository/postgres/UserRepository.cpp
        src/repository/postgres/UserRepository.h
        src/exceptions/exceptions.h
        src/exceptions/repository_exceptions.h
        src/service/UserService.cpp
        src/service/UserService.h
        src/logger/Logger.cpp
        src/logger/Logger.h
        src/exceptions/service_exceptions.h
        src/handler/UserHandler.h
        src/handler/UserHandler.cpp
        src/helpers/ResponseUtils.h
        src/repository/ISessionRepository.h
        src/domain/Session.h
        src/repository/postgres/SessionRepository.cpp
        src/repository/postgres/SessionRepository.h
        src/domain/Chat.h
        src/domain/ChatMember.h
        src/domain/Message.h
        src/service/AuthService.cpp
        src/service/AuthService.h
        src/handler/AuthHandler.cpp
        src/handler/AuthHandler.h
        src/middleware/AuthMiddleware.cpp
        src/middleware/AuthMiddleware.h
        src/context/AuthContext.h
        src/repository/IChatRepository.h
        src/repository/IChatMemberRepository.h
        src/repository/postgres/ChatRepository.cpp
        src/repository/postgres/ChatRepository.h
        src/repository/postgres/ChatMemberRepository.cpp
        src/repository/postgres/ChatMemberRepository.h
        src/service/ChatService.cpp
        src/service/ChatService.h
        src/repository/FactoryRepository.h
        src/service/FactoryService.h
        src/handler/FactoryHandler.h
        src/handler/ChatHandler.cpp
        src/handler/ChatHandler.h
        src/repository/IMessageRepository.h
        src/repository/postgres/MessageRepository.cpp
        src/repository/postgres/MessageRepository.h
        src/service/MessageService.cpp
        src/service/MessageService.h
        src/handler/MessageHandler.cpp
        src/handler/MessageHandler.h
        src/migrations/Migrator.h
        src/helpers/PasswordHasher.h
)

# Добавляем пути к заголовочным файлам
target_include_directories(MessagerV1 PRIVATE
        "C:/vcpkg/installed/x64-mingw-dynamic/include"
)
find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(MessagerV1 unofficial-sodium::sodium)

# Подключаем системные библиотеки Windows
target_link_libraries(MessagerV1 ws2_32 mswsock)

# Подключаем spdlog
target_link_libraries(MessagerV1 spdlog::spdlog)

# Подключаем SOCI (как у вас уже было)
target_link_libraries(MessagerV1
        "C:/vcpkg/installed/x64-mingw-dynamic/lib/libsoci_core_4_0.dll.a"
        "C:/vcpkg/installed/x64-mingw-dynamic/lib/libsoci_postgresql_4_0.dll.a"
)

add_custom_command(TARGET MessagerV1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/migrations
        $<TARGET_FILE_DIR:MessagerV1>/migrations
)

# Компилятор флаги для Windows
if(WIN32)
    target_compile_definitions(MessagerV1 PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )
endif()