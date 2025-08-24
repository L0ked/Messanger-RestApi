#ifndef CHATREPOSITORY_H
#define CHATREPOSITORY_H

#include "repository/IChatRepository.h"

class ChatRepository final: public IChatRepository{
    soci::session & _session;
public:
    explicit ChatRepository(soci::session & session): _session(session) {}
    ~ChatRepository() override = default;

    std::optional<Chat> getById(int id) override;
    std::optional<std::vector<Chat> > getByName(std::string chat_name) override;
    std::optional<Chat> findPersonalChat(int user_id1, int user_id2) override;
    int create(Chat & chat) override;

    int createChatAndChatMembers(Chat &chat, std::vector<ChatMember> &chat_members) override;
    void update(Chat & chat) override;
    void remove(int id) override;
};



#endif //CHATREPOSITORY_H
