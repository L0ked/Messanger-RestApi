#ifndef ICHATMEMBERREPOSITORY_H
#define ICHATMEMBERREPOSITORY_H

#include "domain/ChatMember.h"
#include "domain/Chat.h"
#include "domain/User.h"
#include <optional>

class IChatMemberRepository {
public:
    virtual ~IChatMemberRepository() = default;
    virtual std::optional<std::vector<Chat>> getChatsByUserId(int user_id) = 0;
    virtual std::optional<std::vector<User>> getUserByChatId(int chat_id) = 0;
    virtual std::optional<ChatMember> getByChatAndUser(int chat_id, int user_id) = 0;
    virtual bool exists(int chat_id, int user_id) = 0;
    virtual void create(ChatMember &chat_member) = 0;
    virtual void promoteToAdmin(int chat_id, int user_id) = 0;
    virtual void remove(int chat_id, int user_id) = 0;
};

#endif //ICHATMEMBERREPOSITORY_H
