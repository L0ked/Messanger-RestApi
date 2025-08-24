#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "logger/Logger.h"
#include "exceptions/service_exceptions.h"
#include "repository/IMessageRepository.h"
#include "service/ChatService.h"

class MessageService {
    std::shared_ptr<IMessageRepository> _repository;
    std::shared_ptr<ChatService> _serviceChat;
    std::shared_ptr<spdlog::logger> _log;
public:
    MessageService(std::shared_ptr<IMessageRepository> repository,
        std::shared_ptr<ChatService> serviceChat,
        std::shared_ptr<spdlog::logger> logger) :
    _repository(std::move(repository)), _serviceChat(std::move(serviceChat)),
    _log(std::move(logger)) {}

    std::optional<Message> getById(int id, int requested_by_id);
    std::optional<std::vector<Message>> getByChatId(int chat_id, int limit, int offset, int requested_by_id);
    std::optional<std::vector<Message>> getMessagesAfter(int chat_id, int message_id, int requested_by_id);
    std::optional<std::vector<Message>> getMessagesBefore(int chat_id, int message_id, int limit, int requested_by_id);
    std::optional<Message> getLastMessageInChat(int chat_id, int requested_by_id);

    Message send(int chat_id, int requested_by_id, std::string content);
    void edit(int id, std::string content, int requested_by_id);
    void remove(int id, int requested_by_id);

    Message ensureMessageOwnerOrAdmin(int id, int requested_by_id);
};



#endif //MESSAGESERVICE_H
