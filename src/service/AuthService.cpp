#include "AuthService.h"

std::optional<Session> AuthService::getById(int id) {
    try {
        auto session = this->_repoSession->getById(id);
        if (session == std::nullopt)
            this->_log->debug("Not found session by id = {}", id);
        else
            this->_log->info("Got session by id = {}", id);
        return session;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<Session> AuthService::getByToken(std::string token) {
    try {
        auto session = this->_repoSession->getByToken(token);
        if (session == std::nullopt)
            this->_log->debug("Not found session by token (first 6 chars) = {}", token.substr(0, 6));
        else
            this->_log->info("Got session by token (first 6 chars) = {}", token.substr(0, 6));
        return session;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}


Session AuthService::createSession(int user_id) {
    try {
        const std::time_t localTime = std::time(nullptr);
        const auto token = generateToken(128);

        Session session(0, user_id, token, localTime);
        int id = this->_repoSession->create(session);

        this->_log->info("Created session with id = {}", id);
        session.id = id;

        return session;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void AuthService::removeSession(int id) {
    try {
        this->_repoSession->remove(id);
        this->_log->info("Remove session by id = {}", id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void AuthService::removeAllSession(int user_id) {
    try {
        this->_repoSession->removeAllSession(user_id);
        this->_log->info("Remove all session by user_id = {}", user_id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<int> AuthService::validateToken(std::string token) {
    try {
        auto session = this->getByToken(token);
        if (session == std::nullopt) {
            this->_log->debug("Not valid token");
            return std::nullopt;
        }
        this->_log->info("Valid token");
        return session->user_id;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

Session AuthService::registerUser(std::string username, std::string password, std::string display_name) {
    try {
        auto user = this->_serviceUser->create(username, password, display_name);
        auto session = this->createSession(user.id);
        this->_log->info("Register user by username = {}", username);
        return session;
    } catch (const UsernameTakenException &ex) {
        this->_log->debug("Error register: {}", ex.what());
        throw;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<Session> AuthService::login(std::string username, std::string password) {
    try {
        auto user = this->_serviceUser->getByUsername(username);
        if (user == std::nullopt) {
            this->_log->debug("Session not created, not found user");
            return std::nullopt;
        }
        if (user->password != password) {
            this->_log->debug("Incorrect password by username = {}", username);
            return std::nullopt;
        }

        auto session = this->createSession(user->id);
        this->_log->info("Login user by username = {}", username);
        return session;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::string AuthService::generateToken(int length) {
    const std::string charset =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    // Получаем текущее время
    const auto timeSeed = static_cast<unsigned long>(std::time(nullptr));

    std::random_device rd;
    std::mt19937 gen(rd() ^ timeSeed); // комбинируем сиды
    std::uniform_int_distribution<> dist(0, charset.size() - 1);

    std::string token;
    token.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        token += charset[dist(gen)];
    }
    return token;
}
