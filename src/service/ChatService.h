#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "repository/IChatRepository.h"
#include "repository/IChatMemberRepository.h"
#include "logger/Logger.h"
#include "exceptions/service_exceptions.h"

class ChatService {
    std::shared_ptr<IChatRepository> _repoChat;
    std::shared_ptr<IChatMemberRepository> _repoChatMember;
    std::shared_ptr<spdlog::logger> _log;
public:
    explicit ChatService(std::shared_ptr<IChatRepository> repoChat,
        std::shared_ptr<IChatMemberRepository> repoChatMember,
        std::shared_ptr<spdlog::logger> log) :
    _repoChat(std::move(repoChat)), _repoChatMember(std::move(repoChatMember)),
    _log(std::move(log)) {}

    std::optional<Chat> getChatByChatId(int chat_id, int requested_by_id);
    // std::optional<std::vector<Chat>> getChatByName(std::string name);
    std::optional<std::vector<User>> getUsersByChat(int chat_id, int requested_by_id);
    std::optional<std::vector<Chat>> getChatsByUser(int user_id);

    std::optional<Chat> createPersonal(int user_id1, int user_id2);
    std::optional<Chat> createGroup(std::string name, std::string description, std::vector<int> member_ids);

    void updateGroupName(int chat_id, std::string name, int requested_by_id);
    void updateGroupDescription(int chat_id, std::string description, int requested_by_id);

    void addUserToGroup(int chat_id, int user_id, int requested_by_id);
    void leaveGroupChat(int chat_id, int user_id, int requested_by_id);

    void promoteToAdmin(int chat_id, int user_to_promote_id, int requested_by_id);

    void removeChat(int chat_id, int requested_by_id);


    Chat chatExistsAndNotPersonal(int chat_id);
    void ensureUserInChat(int chat_id, int user_id);
    void ensureAdminChatMember(int chat_id, int requested_by_id);
};



#endif //CHATSERVICE_H
