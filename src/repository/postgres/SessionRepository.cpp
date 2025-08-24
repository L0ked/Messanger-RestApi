#include "SessionRepository.h"

std::optional<Session> SessionRepository::getById(int id) {
    try {
        Session session;
        this->_session << "select * from sessions where id = :id",
                soci::into(session),
                soci::use(id);
        return session;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<Session> SessionRepository::getByToken(std::string token) {
    try {
        Session session;
        this->_session << "select * from sessions where token = :token",
                soci::into(session),
                soci::use(token);
        return session;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

int SessionRepository::create(Session &session) {
    try {
        int id;
        this->_session << "insert into sessions(user_id, token, created_at) "
            "values(:user_id, :token, :created_at) returning id", soci::use(session), soci::into(id);
        return id;
    } catch (const soci::soci_error &) {
        throw;
    }

}

void SessionRepository::remove(int id){
    try {
        this->_session << "delete from sessions where id = :id", soci::use(id);
    } catch (const soci::soci_error &) {
        throw;
    }
}

void SessionRepository::removeAllSession(int user_id) {
    try {
        this->_session << "delete from sessions where user_id = :user_id", soci::use(user_id);
    } catch (const soci::soci_error &) {
        throw;
    }
}
