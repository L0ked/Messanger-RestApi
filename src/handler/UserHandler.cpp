#include "UserHandler.h"


void UserHandler::registerRouters(crow::App<AuthMiddleware> &app) {

    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int id) {
        return this->getById(req, id);
    });

    CROW_ROUTE(app, "/users/<string>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, std::string username) {
        return this->getByUsername(req, username);
    });

    CROW_ROUTE(app, "/users").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->create(req);
    });

    CROW_ROUTE(app, "/me/change-password").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->change_password(req);
    });

    CROW_ROUTE(app, "/me").methods(crow::HTTPMethod::PATCH)
    ([this](const crow::request &req) {
        return this->update(req);
    });

    CROW_ROUTE(app, "/me").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req) {
        return this->remove_self(req);
    });
}

crow::response UserHandler::getById(const crow::request &req, int id) {
    try {
        auto user = this->_service->getById(id);
        if (user == std::nullopt)
            return error_response(404, "User not found");
        return crow::response(200, user->to_json());
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response UserHandler::getByUsername(const crow::request &req, std::string username) {
    try {
        auto user = this->_service->getByUsername(username);
        if (user == std::nullopt)
            return error_response(404, "User not found");
        return crow::response(200, user->to_json());
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response UserHandler::create(const crow::request &req) {
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

        auto user = this->_service->create(username, password, display_name);

        return crow::response(201, user.to_json());

    } catch (const UsernameTakenException &) {
        return error_response(409, "Username taken");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response UserHandler::change_password(const crow::request &req) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        int id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body)
            return error_response(400, "Invalid JSON");

        if (!body.has("old_password") || body["old_password"].t() != crow::json::type::String ||
            !body.has("new_password") || body["new_password"].t() != crow::json::type::String ) {
            return error_response(400, "Missing or invalid 'old_password' or 'new_password'");
        }

        std::string old_password = body["old_password"].s();
        std::string new_password = body["new_password"].s();

        auto user = this->_service->getById(id);

        if (!user.has_value() || !PasswordHasher::verify(old_password, user->password)) {
            return error_response(401, "Incorrect password");
        }

        this->_service->update_password(id, new_password);
        return crow::response(200);

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response UserHandler::update(const crow::request &req) {
    try {
        auto body = crow::json::load(req.body);
        if (!body)
            return error_response(400, "Invalid JSON");
        if (body.has("display_name") && body["display_name"].t() == crow::json::type::String) {
            auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
            int id = ctx.user_id.value();

            std::string display_name = body["display_name"].s();
            this->_service->update_display_name(id, display_name);

            return crow::response(200);
        }
        return crow::response(304);

    } catch (const UserNotFoundException &) {
        return error_response(404, "User not found");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response UserHandler::remove_self(const crow::request &req) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        int id = ctx.user_id.value();

        this->_service->remove(id);

        return crow::response(200);

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}
