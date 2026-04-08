#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <memory>
#include <utility>

#include "repository/IUserRepository.h"
#include "logger/Logger.h"

#include "helpers/PasswordHasher.h"
#include "exceptions/repository_exceptions.h"
#include "exceptions/service_exceptions.h"

class UserService {
    std::shared_ptr<IUserRepository> _repository;
    std::shared_ptr<spdlog::logger> _log;

public:
    UserService(std::shared_ptr<IUserRepository> repository, std::shared_ptr<spdlog::logger> log):
        _repository(std::move(repository)), _log(std::move(log)) {}

    std::optional<User> getById(int id);
    std::optional<User> getByUsername(std::string username);

    User create(std::string username, std::string password, std::string display_name);
    void update_password(int id, std::string new_password);
    void update_display_name(int id, std::string display_name);
    void remove(int id);
};


#endif //USERSERVICE_H
