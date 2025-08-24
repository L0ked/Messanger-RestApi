#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include "crow.h"

#include "service/ChatService.h"
#include "middleware/AuthMiddleware.h"
#include "helpers/ResponseUtils.h"

class ChatHandler {
    std::shared_ptr<ChatService> _service;
public:
    explicit ChatHandler(std::shared_ptr<ChatService> service):
        _service(std::move(service)) {}

    void registerRouters(crow::App<AuthMiddleware> & app);

    crow::response getChatByChatId(const crow::request &req, int chat_id);
    crow::response getUsersByChat(const crow::request &req, int chat_id);
    crow::response getChatsByUser(const crow::request &req);

    crow::response createPersonal(const crow::request &req);
    crow::response createGroup(const crow::request &req);

    crow::response updateGroup(const crow::request &req, int chat_id);

    crow::response addUserToGroup(const crow::request &req, int chat_id, int user_id);
    crow::response leaveGroupChat(const crow::request &req, int chat_id, int user_id);
    crow::response promoteToAdmin(const crow::request &req, int chat_id, int user_id);

    crow::response removeChat(const crow::request &req, int chat_id);
};



#endif //CHATHANDLER_H
