#ifndef REPOSITORY_EXCEPTIONS_H
#define REPOSITORY_EXCEPTIONS_H

#include "exceptions.h"

struct RepositoryException : AppException {
    explicit RepositoryException(const std::string &message)
        : AppException("repository exception: " + message) {
    }
};

struct UniqueConstraintException : RepositoryException {
    explicit UniqueConstraintException(const std::string &message)
        : RepositoryException("unique violation: " + message) {
    }
};
#endif //REPOSITORY_EXCEPTIONS_H
