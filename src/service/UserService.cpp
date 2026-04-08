#include "UserService.h"


std::optional<User> UserService::getById(int id) {
    try {
        auto user = this->_repository->getById(id);
        if (user == std::nullopt)
            this->_log->debug("Not found user by id = {}", id);
        else
            this->_log->info("Got user by id = {}", id);
        return user;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<User> UserService::getByUsername(std::string username) {
    try {
        auto user = this->_repository->getByUsername(username);
        if (user == std::nullopt)
            this->_log->debug("Not found user by username = {}", username);
        else
            this->_log->info("Get user by username = {}", username);
        return user;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

User UserService::create(std::string username, std::string password, std::string display_name) {
    try {
        User user(0, username, PasswordHasher::hash(password),display_name, std::time(nullptr));
        int id = this->_repository->create(user);

        this->_log->info("Created user with id = {}", id);
        user.id = id;

        return user;
    } catch (const UniqueConstraintException &) {
        this->_log->debug("Username Taken: {}", username);
        throw UsernameTakenException(username);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void UserService::update_password(int id, std::string new_password) {
    try {
        this->_repository->update_password(id, PasswordHasher::hash(new_password));
        this->_log->info("Update password with id = {}", id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void UserService::update_display_name(int id, std::string display_name) {
    try {
        auto user = this->_repository->getById(id);
        if (user == std::nullopt) {
            this->_log->debug("Not found user by id = {}", id);
            throw UserNotFoundException(std::to_string(id));
        }
        this->_log->info("Get user by id = {}", id);

        user->display_name = std::move(display_name);
        this->_repository->update(user.value());
        this->_log->info("Update display name by user_id = {}", id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void UserService::remove(int id) {
    try {
        this->_repository->remove(id);
        this->_log->info("Remove user by id = {}", id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}
