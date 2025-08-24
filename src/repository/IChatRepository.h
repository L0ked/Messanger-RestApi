#ifndef ICHATREPOSITORY_H
#define ICHATREPOSITORY_H

#include "domain/Chat.h"
#include "domain/ChatMember.h"
#include <optional>

class IChatRepository {
public:
    virtual ~IChatRepository() = default;
    virtual std::optional<Chat> getById(int id) = 0;
    virtual std::optional<std::vector<Chat> > getByName(std::string chat_name) = 0;
    virtual std::optional<Chat> findPersonalChat(int user_id1, int user_id2) = 0;
    virtual int create(Chat &chat) = 0;
    virtual int createChatAndChatMembers(Chat &chat, std::vector<ChatMember> &chat_members) = 0;
    virtual void update(Chat &chat) = 0;
    virtual void remove(int id) = 0;
};

#endif //ICHATREPOSITORY_H
