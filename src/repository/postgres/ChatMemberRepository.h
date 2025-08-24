#ifndef CHATMEMBERREPOSITORY_H
#define CHATMEMBERREPOSITORY_H

#include "repository/IChatMemberRepository.h"

class ChatMemberRepository final: public IChatMemberRepository{
    soci::session & _session;
public:
    explicit ChatMemberRepository(soci::session & session): _session(session) {}
    ~ChatMemberRepository() override = default;

    std::optional<std::vector<Chat>> getChatsByUserId(int user_id) override;
    std::optional<std::vector<User>> getUserByChatId(int chat_id) override;
    std::optional<ChatMember> getByChatAndUser(int chat_id, int user_id) override;
    bool exists(int chat_id, int user_id) override;
    void create(ChatMember &chat_member) override;
    void promoteToAdmin(int chat_id, int user_id) override;
    void remove(int chat_id, int user_id) override;
};



#endif //CHATMEMBERREPOSITORY_H
