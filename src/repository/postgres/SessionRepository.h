#ifndef SESSIONREPOSITORY_H
#define SESSIONREPOSITORY_H

#include "repository/ISessionRepository.h"

class SessionRepository: public ISessionRepository{
    soci::session & _session;
public:
    explicit SessionRepository(soci::session & session) : _session(session) {}
    ~SessionRepository() override = default;
    std::optional<Session> getById(int id) override;
    std::optional<Session> getByToken(std::string token) override;
    int create(Session & session) override;
    void remove(int id) override;
    void removeAllSession(int user_id) override;
};



#endif //SESSIONREPOSITORY_H
