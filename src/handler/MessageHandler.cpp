#include "MessageHandler.h"

void MessageHandler::registerRouters(crow::App<AuthMiddleware> &app) {
    CROW_ROUTE(app, "/messages/<int>").methods(crow::HTTPMethod::GET)
   ([this](const crow::request &req, int id) {
       return this->getById(req, id);
   });

    CROW_ROUTE(app, "/chats/<int>/messages").methods(crow::HTTPMethod::GET)
   ([this](const crow::request &req, int chat_id) {
       return this->getByChatId(req, chat_id);
   });

    CROW_ROUTE(app, "/chats/<int>/messages/after/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int chat_id, int message_id) {
        return this->getMessagesAfter(req, chat_id, message_id);
    });

    CROW_ROUTE(app, "/chats/<int>/messages/before/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int chat_id, int message_id) {
        return this->getMessagesBefore(req, chat_id, message_id);
    });

    CROW_ROUTE(app, "/chats/<int>/messages/last").methods(crow::HTTPMethod::GET)
    ([this](const crow::request &req, int chat_id) {
        return this->getLastMessageInChat(req, chat_id);
    });

    CROW_ROUTE(app, "/chats/<int>/messages").methods(crow::HTTPMethod::POST)
    ([this](const crow::request &req, int chat_id) {
        return this->send(req, chat_id);
    });

    CROW_ROUTE(app, "/messages/<int>").methods(crow::HTTPMethod::PATCH)
    ([this](const crow::request &req, int id) {
        return this->edit(req, id);
    });

    CROW_ROUTE(app, "/messages/<int>").methods(crow::HTTPMethod::Delete)
    ([this](const crow::request &req, int id) {
        return this->remove(req, id);
    });
}

crow::response MessageHandler::getById(const crow::request &req, int id) {
    try{
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto message = this->_service->getById(id, requested_by_id);
        if (!message.has_value()) {
            return error_response(404, "Message not found");
        }
        return crow::response(200, message->to_json());

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::getByChatId(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto limit = parseIntParam(req, "limit", 50, 1, 100);
        auto offset = parseIntParam(req, "offset", 0, 0, INT_MAX);
        if (!limit.has_value()) {
            return error_response(400, "Invalid limit parameter");
        }
        if (!offset.has_value()) {
            return error_response(400, "Invalid offset parameter");
        }

        auto messages = this->_service->getByChatId(chat_id, limit.value(), offset.value(), requested_by_id);
        if (!messages.has_value()) {
            return error_response(404, "Messages not found");
        }

        crow::json::wvalue result;
        crow::json::wvalue::list messages_json;

        for (Message & message: messages.value()) {
            messages_json.push_back(message.to_json());
        }
        result["messages"] = std::move(messages_json);
        return crow::response(200, result);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::getMessagesAfter(const crow::request &req, int chat_id, int id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto messages = this->_service->getMessagesAfter(chat_id, id, requested_by_id);
        if (!messages.has_value()) {
            return error_response(404, "Messages not found");
        }

        crow::json::wvalue result;
        crow::json::wvalue::list messages_json;

        for (Message & message: messages.value()) {
            messages_json.push_back(message.to_json());
        }
        result["messages"] = std::move(messages_json);
        return crow::response(200, result);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::getMessagesBefore(const crow::request &req, int chat_id, int id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto limit = parseIntParam(req, "limit", 50, 1, 100);
        if (!limit.has_value()) {
            return error_response(400, "Invalid limit parameter");
        }

        auto messages = this->_service->getMessagesBefore(chat_id, id, limit.value(), requested_by_id);
        if (!messages.has_value()) {
            return error_response(404, "Messages not found");
        }

        crow::json::wvalue result;
        crow::json::wvalue::list messages_json;

        for (Message & message: messages.value()) {
            messages_json.push_back(message.to_json());
        }
        result["messages"] = std::move(messages_json);
        return crow::response(200, result);

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::getLastMessageInChat(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto message = this->_service->getLastMessageInChat(chat_id, requested_by_id);
        if (!message.has_value()) {
            return error_response(404, "Message not found");
        }
        return crow::response(200, message->to_json());

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::send(const crow::request &req, int chat_id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body) {
            return error_response(400, "Invalid JSON");
        }
        if (!body.has("content") || body["content"].t() != crow::json::type::String) {
            return error_response(400, "Missing or invalid 'content'");
        }
        std::string content = body["content"].s();

        auto message = this->_service->send(chat_id, requested_by_id, content);

        return crow::response(201, message.to_json());

    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::constraint_violation) {
            return error_response(400, "Chat not exist");
        }
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::edit(const crow::request &req, int id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        auto body = crow::json::load(req.body);
        if (!body) {
            return error_response(400, "Invalid JSON");
        }
        if (!body.has("content") || body["content"].t() != crow::json::type::String) {
            return error_response(400, "Missing or invalid 'content'");
        }
        std::string content = body["content"].s();

        this->_service->edit(id, content, requested_by_id);

        return crow::response(200);

    } catch (const MessageNotFoundException &) {
        return error_response(404, "Message not found");
    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const PermissionException &) {
        return error_response(403, "Sending user not Admin");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

crow::response MessageHandler::remove(const crow::request &req, int id) {
    try {
        auto& ctx = *static_cast<AuthContext*>(req.middleware_context);
        const int requested_by_id = ctx.user_id.value();

        this->_service->remove(id, requested_by_id);

        return crow::response(204);

    } catch (const MessageNotFoundException &) {
        return error_response(404, "Message not found");
    } catch (const ChatMemberNotFoundException &) {
        return error_response(403, "Sending user not in chat");
    } catch (const PermissionException &) {
        return error_response(403, "Sending user not Admin");
    } catch (const soci::soci_error &) {
        return error_response(500, "Internal error");
    }
}

std::optional<int> MessageHandler::parseIntParam(const crow::request &req, std::string param_name, int default_value,
                                                 int min_value, int max_value)
{
    auto param = req.url_params.get(param_name);
    if (param == nullptr) {
        return default_value;
    }
    try {
        int value = std::stoi(param);
        if (value < min_value) return min_value;
        if (value > max_value) return max_value;
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}
