#ifndef SERVICE_EXCEPTIONS_H
#define SERVICE_EXCEPTIONS_H

#include "exceptions.h"

struct ServiceException: AppException {
    explicit ServiceException(const std::string & message):
        AppException("service exception: " + message) {}
};

struct UsernameTakenException : ServiceException {
    explicit UsernameTakenException(const std::string & message)
        : ServiceException("username taken: " + message) {}
};

struct UserNotFoundException : ServiceException {
    explicit UserNotFoundException(const std::string & message)
        : ServiceException("not found user: " + message) {}
};

struct ChatNotFoundException : ServiceException {
    explicit ChatNotFoundException(const std::string & message)
        : ServiceException("not found chat: " + message) {}
};

struct ChatMemberNotFoundException : ServiceException {
    explicit ChatMemberNotFoundException(const std::string & message)
        : ServiceException("not found chatmember: " + message) {}
};

struct PermissionException: ServiceException {
    explicit PermissionException(const std::string & message)
        : ServiceException("don`t have enough permissions: " + message) {}
};

struct PersonalChatException: ServiceException {
    explicit PersonalChatException(const std::string & message)
        : ServiceException("Personal chat cannot change: " + message) {}
};

struct MessageNotFoundException: ServiceException {
    explicit MessageNotFoundException(const std::string & message)
        : ServiceException("not found message: " + message) {}
};

#endif //SERVICE_EXCEPTIONS_H
