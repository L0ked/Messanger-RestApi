#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H


#include "repository/IUserRepository.h"
#include "domain/User.h"

class UserRepository: public IUserRepository {
    soci::session & _session;
public:
    explicit UserRepository(soci::session & session): _session(session) {}
    ~UserRepository() override = default;
    std::optional<User> getById(int id) override;
    std::optional<User> getByUsername(std::string username) override;
    int create(const User & user) override;
    void update(const User & user) override;
    void update_password(int id, std::string password) override;
    void remove(int id) override;
};



#endif //USERREPOSITORY_H
