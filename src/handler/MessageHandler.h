#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "crow.h"

#include "service/MessageService.h"
#include "middleware/AuthMiddleware.h"
#include "helpers/ResponseUtils.h"

class MessageHandler {
    std::shared_ptr<MessageService> _service;
public:
    explicit MessageHandler(std::shared_ptr<MessageService> service):
        _service(std::move(service)) {}

    void registerRouters(crow::App<AuthMiddleware> & app);

    crow::response getById(const crow::request &req, int id);
    crow::response getByChatId(const crow::request &req, int chat_id);
    crow::response getMessagesAfter(const crow::request &req, int chat_id, int id);
    crow::response getMessagesBefore(const crow::request &req, int chat_id, int id);
    crow::response getLastMessageInChat(const crow::request &req, int chat_id);

    crow::response send(const crow::request &req, int chat_id);
    crow::response edit(const crow::request &req, int id);
    crow::response remove(const crow::request &req, int id);

    std::optional<int> parseIntParam(const crow::request &req, std::string param_name,
        int default_value, int min_value, int max_value);
};



#endif //MESSAGEHANDLER_H
