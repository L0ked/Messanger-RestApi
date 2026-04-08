#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "repository/ISessionRepository.h"
#include "service/UserService.h"
#include "helpers/PasswordHasher.h"
#include "logger/Logger.h"

class AuthService {
    std::shared_ptr<ISessionRepository> _repoSession;
    std::shared_ptr<UserService> _serviceUser;
    std::shared_ptr<spdlog::logger> _log;
public:
    AuthService(std::shared_ptr<ISessionRepository> repoSession, std::shared_ptr<UserService> serviceUser,
        std::shared_ptr<spdlog::logger> log): _repoSession(std::move(repoSession)),
        _serviceUser(std::move(serviceUser)), _log(std::move(log)) {}

    std::optional<Session> getById(int id);
    std::optional<Session> getByToken(std::string token);

    Session createSession(int user_id);
    void removeSession(int id);
    void removeAllSession(int user_id);

    std::optional<int> validateToken(std::string token); // return user_id

    Session registerUser(std::string username, std::string password, std::string display_name);
    std::optional<Session> login(std::string username, std::string password);
private:
    static std::string generateToken(int length);
};



#endif //AUTHSERVICE_H
