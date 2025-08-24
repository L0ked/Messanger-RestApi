#ifndef IMESSAGEREPOSITORY_H
#define IMESSAGEREPOSITORY_H

#include "domain/Message.h"
#include <optional>

class IMessageRepository {
public:
    virtual ~IMessageRepository() = default;

    virtual std::optional<Message> getById(int id) = 0;
    virtual std::optional<std::vector<Message>> getByChatId(int chat_id, int limit, int offset) = 0;
    virtual std::optional<std::vector<Message>> getMessagesAfter(int chat_id, int message_id) = 0;
    virtual std::optional<std::vector<Message>> getMessagesBefore(int chat_id, int message_id, int limit) = 0;
    virtual std::optional<Message> getLastMessageInChat(int chat_id) = 0;

    virtual int create(Message & message) = 0;
    virtual void update(Message & message) = 0;
    virtual void remove(int id) = 0;
};

#endif //IMESSAGEREPOSITORY_H
