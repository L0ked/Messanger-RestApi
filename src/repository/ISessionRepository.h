#ifndef ISESSIONREPOSITORY_H
#define ISESSIONREPOSITORY_H

#include "domain/Session.h"
#include <optional>

class ISessionRepository {
public:
    virtual ~ISessionRepository() = default;
    virtual std::optional<Session> getById(int id) = 0;
    virtual std::optional<Session> getByToken(std::string token) = 0;
    virtual int create(Session & session) = 0;
    virtual void remove(int id) = 0;
    virtual void removeAllSession(int user_id) = 0;
};
#endif //ISESSIONREPOSITORY_H
