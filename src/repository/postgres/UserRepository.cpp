#include <string>
#include <soci/soci.h>

#include "UserRepository.h"
#include "domain/User.h"
#include "exceptions/repository_exceptions.h"


std::optional<User> UserRepository::getById(int id) {
    try {

        const soci::rowset<User> rs = (this->_session.prepare << "select * from users where id = :id",
                soci::use(id));
        for (const auto& row : rs) {
            return row;
        }
        return std::nullopt;

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<User> UserRepository::getByUsername(std::string username) {
    try {
        const soci::rowset<User> rs = (this->_session.prepare << "select * from users where username = :username",
                soci::use(username));
        for (const auto& row : rs) {
            return row;
        }
        return std::nullopt;

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

int UserRepository::create(const User &user) {
    try {
        int id;
        this->_session << "insert into users(username, password, display_name, created_at) "
                "values(:username, :password, :display_name, :created_at) returning id",
                soci::use(user), soci::into(id);
        return id;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation)
            throw UniqueConstraintException(ex.what());
        throw;
    }
}

void UserRepository::update(const User &user) {
    try {
        this->_session << "update users set username = :username, password = :password, display_name = :display_name "
                "where id = :id", soci::use(user);
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation)
            throw UniqueConstraintException(ex.what());
        throw;
    }
}

void UserRepository::update_password(int id, std::string password) {
    try {
        this->_session << "update users set password = :password "
                "where id = :id", soci::use(password), soci::use(id);
    } catch (const soci::soci_error &ex) {
        throw;
    }
}

void UserRepository::remove(int id) {
    try {
       this->_session << "delete from users where id = :id", soci::use(id);
    } catch (const soci::soci_error &) {
        throw;
    }
}
