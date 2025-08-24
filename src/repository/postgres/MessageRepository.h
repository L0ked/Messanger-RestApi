#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include "repository/IMessageRepository.h"

class MessageRepository final: public IMessageRepository {
    soci::session & _session;
public:
    explicit MessageRepository(soci::session & session): _session(session) {}
    ~MessageRepository() override = default;

    std::optional<Message> getById(int id) override;
    std::optional<std::vector<Message>> getByChatId(int chat_id, int limit, int offset) override;
    std::optional<std::vector<Message>> getMessagesAfter(int chat_id, int message_id) override;
    std::optional<std::vector<Message>> getMessagesBefore(int chat_id, int message_id, int limit) override;
    std::optional<Message> getLastMessageInChat(int chat_id) override;

    int create(Message & message) override;
    void update(Message & message) override;
    void remove(int id) override;
};



#endif //MESSAGEREPOSITORY_H
