#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

struct AppException: std::runtime_error {
    explicit AppException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif //EXCEPTIONS_H
