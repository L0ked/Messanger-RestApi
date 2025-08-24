#include "ChatHandler.h"

void ChatHandler::registerRouters(crow::App<AuthMiddleware> &app) {
    CROW_ROUTE(app, "/chats/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int chat_id) {
        return this->getChatByChatId(req, chat_id);
    });

    CROW_ROUTE(app, "/chats/<int>/users").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int chat_id) {
        return this->getUsersByChat(req, chat_id);
    });

    CROW_ROUTE(app, "/me/chats").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req) {
        return this->getChatsByUser(req);
    });

    CROW_ROUTE(app, "/chats/personal").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->createPersonal(req);
    });

    CROW_ROUTE(app, "/chats/group").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req) {
        return this->createGroup(req);
    });

    CROW_ROUTE(app, "/chats/<int>").methods(crow::HTTPMethod::PATCH)
    ([this](const crow::request &req, int chat_id) {
        return this->updateGroup(req, chat_id);
    });

    CROW_ROUTE(app, "/chats/<int>/users/<int>").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req, int chat_id, int user_id) {
        return this->addUserToGroup(req, chat_id, user_id);
    });

    CROW_ROUTE(app, "/chats/<int>/users/<int>").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req, int chat_id, int user_id) {
        return this->leaveGroupChat(req, chat_id, user_id);
    });

    CROW_ROUTE(app, "/chats/<int>/users/<int>/promote").methods(crow::HTTPMethod::PATCH)
    ([this](const crow::request &req, int chat_id, int user_id) {
        return this->promoteToAdmin(req, chat_id, user_id);
    });

    CROW_ROUTE(app, "/chats/<int>").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req, int chat_id) {
        return this->removeChat(req, chat_id);
    });
}

crow::response ChatHandler::getChatByChatId(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto chat = this->_service->getChatByChatId(chat_id, requested_by_id);
        if (!chat.has_value()) {
            return error_response(404, "Chat not found");
        }
        return crow::response(200, chat->to_json());

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::getUsersByChat(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto users = this->_service->getUsersByChat(chat_id, requested_by_id);
        if (!users.has_value()) {
            return error_response(404, "Chat not found");
        }
        crow::json::wvalue result;
        crow::json::wvalue::list users_json;

        for (User & user: users.value()) {
            users_json.push_back(user.to_json());
        }
        result["users"] = std::move(users_json);
        return crow::response(200, result);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::getChatsByUser(const crow::request &req) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto chats = this->_service->getChatsByUser(requested_by_id);
        if (!chats.has_value()) {
            crow::json::wvalue result;
            result["chats"] = crow::json::wvalue::list{};
            return crow::response(200, result);
        }
        crow::json::wvalue result;
        crow::json::wvalue::list chats_json;

        for (Chat & chat: chats.value()) {
            chats_json.push_back(chat.to_json());
        }
        result["chats"] = std::move(chats_json);
        return crow::response(200, result);

    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::createPersonal(const crow::request &req) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body) {
            return error_response(400, "Invalid JSON");
        }
        if (!body.has("user_id") || body["user_id"].t() != crow::json::type::Number) {
            return error_response(400, "Missing or invalid 'user_id'");
        }
        int user_id2 = body["user_id"].i();

        auto chat = this->_service->createPersonal(requested_by_id, user_id2);

        return crow::response(201, chat->to_json());

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "User not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::createGroup(const crow::request &req) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body) {
            return error_response(400, "Invalid JSON");
        }
        if (!body.has("user_ids") || body["user_ids"].t() != crow::json::type::List ||
            !body.has("name") || body["name"].t() != crow::json::type::String ||
            !body.has("description") || body["description"].t() != crow::json::type::String) {
            return error_response(400, "Missing or invalid 'user_ids' or 'name' or 'description'");
        }

        std::string name = body["name"].s();
        std::string description = body["description"].s();
        std::vector<int> member_ids;
        member_ids.push_back(requested_by_id);

        for (const auto& item: body["user_ids"]) {
            if (item.t() != crow::json::type::Number) {
                return error_response(400, "All 'user_ids' must be integers");
            }
            member_ids.push_back(item.i());
        }

        auto chat = this->_service->createGroup(name, description, member_ids);
        
        return crow::response(201, chat->to_json());

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "User does not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::updateGroup(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body) {
            return error_response(400, "Invalid JSON");
        }

        if ((!body.has("name") || body["name"].t() != crow::json::type::String) &&
            (!body.has("description") || body["description"].t() != crow::json::type::String) ) {
            return error_response(400, "Missing or invalid 'name' or 'description'");
        }

        if (body.has("name")) {
            std::string name = body["name"].s();
            this->_service->updateGroupName(chat_id, name, requested_by_id);
        }

        if (body.has("description")) {
            std::string description = body["description"].s();
            this->_service->updateGroupDescription(chat_id, description, requested_by_id);
        }

        return crow::response(200);

    } catch (const ChatMemberNotFoundException &) {
            return error_response(403, "Sending user not in chat");
    } catch (const ChatNotFoundException &) {
        return error_response(400, "Chat not exist");
    } catch (const PersonalChatException &){
        return error_response(405, "Personal chat unmodifiable");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "Chat not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::addUserToGroup(const crow::request &req, int chat_id, int user_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        this->_service->addUserToGroup(chat_id, user_id, requested_by_id);
        return crow::response(200);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const ChatNotFoundException &) {
        return error_response(400, "Chat not exist");
    } catch (const PersonalChatException &){
        return error_response(403, "Personal chat cannot add user");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "User or chat not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::leaveGroupChat(const crow::request &req, int chat_id, int user_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        this->_service->leaveGroupChat(chat_id, user_id, requested_by_id);
        return crow::response(200);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const ChatNotFoundException &) {
        return error_response(404, "Chat not exist");
    } catch (const PersonalChatException &){
        return error_response(405, "Personal chat cannot remove user");
    } catch (const PermissionException &) {
        return error_response(403, "Sending user not Admin");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "User or chat not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::promoteToAdmin(const crow::request &req, int chat_id, int user_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        this->_service->promoteToAdmin(chat_id, user_id, requested_by_id);
        return crow::response(200);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const ChatNotFoundException &) {
        return error_response(400, "Chat not exist");
    } catch (const PersonalChatException &){
        return error_response(405, "Personal chat cannot remove user");
    } catch (const PermissionException &) {
        return error_response(403, "Sending user not Admin");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "User or chat not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response ChatHandler::removeChat(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        this->_service->removeChat(chat_id, requested_by_id);
        return crow::response(200);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const ChatNotFoundException &) {
        return error_response(400, "Chat not exist");
    } catch (const PermissionException &) {
        return error_response(403, "Sending user not Admin");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "Chat not exist");
        }
        return error_response(500, "Internal error");
    }
}
