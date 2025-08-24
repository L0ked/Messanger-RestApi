#include "AuthHandler.h"

void AuthHandler::registerRouters(crow::App<AuthMiddleware> &app) {
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->registerUser(req);
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->login(req);
    });

    CROW_ROUTE(app, "/logout").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req) {
        return this->logout(req);
    });

    CROW_ROUTE(app, "/logout-all").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req) {
        return this->logoutAll(req);
    });
}

crow::response AuthHandler::registerUser(const crow::request &req) {
    try {
        auto body = crow::json::load(req.body);
        if (!body)
            return error_response(400, "Invalid JSON");

        if (!body.has("username") || body["username"].t() != crow::json::type::String ||
            !body.has("password") || body["password"].t() != crow::json::type::String ||
            !body.has("display_name") || body["display_name"].t() != crow::json::type::String) {
            return error_response(400, "Missing or invalid 'username' or 'password' or 'display_name'");
        }
        std::string username = body["username"].s();
        std::string password = body["password"].s();
        std::string display_name = body["display_name"].s();

        auto session = this->_service->registerUser(username, password, display_name);

        return crow::response(201, crow::json::wvalue{{"token", session.token}});

    } catch (const UsernameTakenException & ex) {
        return error_response(409, "Username taken");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response AuthHandler::login(const crow::request &req) {
    try {
        auto body = crow::json::load(req.body);
        if (!body)
            return error_response(400, "Invalid JSON");

        if (!body.has("username") || body["username"].t() != crow::json::type::String ||
            !body.has("password") || body["password"].t() != crow::json::type::String) {
            return error_response(400, "Missing or invalid 'username' or 'password'");
        }
        std::string username = body["username"].s();
        std::string password = body["password"].s();

        auto session = this->_service->login(username, password);
        if (session == std::nullopt) {
            return error_response(401, "User not exists or incorrect password");
        }
        return crow::response(201, crow::json::wvalue{{"token", session->token}});

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response AuthHandler::logout(const crow::request &req) {
    try {
        const auto auth_header = req.get_header_value("Authorization");

        if (auth_header.empty() || !auth_header.starts_with("Bearer "))
            return error_response(401, "Missing or malformed Authorization header");

        std::string token = auth_header.substr(7);
        auto session = this->_service->getByToken(token);

        if (session == std::nullopt)
            return error_response(401, "Invalid token");

        this->_service->removeSession(session->id);

        return crow::response(200);

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response AuthHandler::logoutAll(const crow::request &req) {
    try {
        const auto auth_header = req.get_header_value("Authorization");

        if (auth_header.empty() || !auth_header.starts_with("Bearer "))
            return error_response(401, "Missing or malformed Authorization header");

        std::string token = auth_header.substr(7);
        auto session = this->_service->getByToken(token);

        if (session == std::nullopt)
            return error_response(401, "Invalid token");

        this->_service->removeAllSession(session->user_id);

        return crow::response(200);

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}
