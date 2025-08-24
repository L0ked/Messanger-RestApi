#ifndef IUSERREPOSITORY_H
#define IUSERREPOSITORY_H

#include <optional>
#include "domain/User.h"

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<User> getById(int id) = 0;
    virtual std::optional<User> getByUsername(std::string username) = 0;
    virtual int create(const User &user) = 0;
    virtual void update(const User &user) = 0;
    virtual void update_password(int id, std::string password) = 0;
    virtual void remove(int id) = 0;
};



#endif //IUSERREPOSITORY_H
