#include "ChatService.h"


std::optional<Chat> ChatService::getChatByChatId(int chat_id, int requested_by_id) {
    try {
        this->ensureUserInChat(chat_id, requested_by_id);

        auto chat = this->_repoChat->getById(chat_id);
        if (!chat.has_value())
            this->_log->debug("Not found chat by id = {}", chat_id);
        else
            this->_log->info("Got chat by id = {}", chat_id);
        return chat;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<std::vector<User> > ChatService::getUsersByChat(int chat_id, int requested_by_id) {
    try {
        this->ensureUserInChat(chat_id, requested_by_id);

        auto users = this->_repoChatMember->getUserByChatId(chat_id);
        if (!users.has_value())
            this->_log->debug("Not found users by chat_id = {}", chat_id);
        else
            this->_log->info("Got users by chat_id = {}", chat_id);
        return users;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<std::vector<Chat> > ChatService::getChatsByUser(int user_id) {
    try {
        auto chats = this->_repoChatMember->getChatsByUserId(user_id);
        if (!chats.has_value())
            this->_log->debug("Not found chats by user_id = {}", user_id);
        else
            this->_log->info("Got chats by user_id = {}", user_id);
        return chats;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<Chat> ChatService::createPersonal(int user_id1, int user_id2) {
    try {
        auto chat = this->_repoChat->findPersonalChat(user_id1, user_id2);
        if (chat.has_value()) {
            this->_log->debug("Found personal chat by user_id1 = {} and user_id2 = {}", user_id1, user_id2);
            return chat;
        }

        chat = Chat(0, "", "", PERSONAL, std::time(nullptr));
        std::vector<ChatMember> chat_members = {
            ChatMember(0, user_id1, ADMIN, std::time(nullptr)),
            ChatMember(0, user_id2, ADMIN, std::time(nullptr))
        };
        int chat_id = this->_repoChat->createChatAndChatMembers(*chat, chat_members);

        this->_log->info("Created chat with id = {}", chat_id);
        chat.value().id = chat_id;
        this->_log->info("Created chat member1 with chat_id = {} and user_id = {}", chat_id, user_id1);
        this->_log->info("Created chat member2 with chat_id = {} and user_id = {}", chat_id, user_id2);
        return chat;
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<Chat> ChatService::createGroup(std::string name, std::string description, std::vector<int> member_ids) {
        try {
            Chat chat(0, name, description, GROUPING, std::time(nullptr));
            std::vector<ChatMember> chat_members;
            for (int const & user_id: member_ids) {
                if (user_id == member_ids[0])
                    chat_members.push_back(ChatMember(0, user_id, ADMIN, std::time(nullptr)));
                else {
                    chat_members.push_back(ChatMember(0, user_id, MEMBER, std::time(nullptr)));
                }
             }
            int chat_id = this->_repoChat->createChatAndChatMembers(chat, chat_members);
            this->_log->info("Created chat with id = {}", chat_id);

            chat.id = chat_id;
            for (int i = 0; i < member_ids.size(); i++)
                this->_log->info("Created chat member{} with chat_id = {} and user_id = {}", i+1, chat_id, member_ids[i]);
            return chat;
        } catch (const soci::soci_error &ex) {
            this->_log->error("Error Database: {}", ex.what());
            throw;
        }
}

void ChatService::updateGroupName(int chat_id, std::string name, int requested_by_id) {
    try {
        auto chat = this->chatExistsAndNotPersonal(chat_id);

        this->ensureUserInChat(chat_id, requested_by_id);

        chat.name = name;
        this->_repoChat->update(chat);
        this->_log->info("Update name by chat_id = {}", chat_id);

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void ChatService::updateGroupDescription(int chat_id, std::string description, int requested_by_id) {
    try {
        auto chat = this->chatExistsAndNotPersonal(chat_id);

        this->ensureUserInChat(chat_id, requested_by_id);

        chat.description = description;
        this->_repoChat->update(chat);
        this->_log->info("Update description by chat_id = {}", chat_id);

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void ChatService::addUserToGroup(int chat_id, int user_id, int requested_by_id) {
    try {
        this->chatExistsAndNotPersonal(chat_id);

        this->ensureUserInChat(chat_id, requested_by_id);

        ChatMember user_member(chat_id, user_id, MEMBER, std::time(nullptr));
        this->_repoChatMember->create(user_member);
        this->_log->info("Add user by user_id = {} to chat by chat_id = {}", user_id, chat_id);

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}


void ChatService::leaveGroupChat(int chat_id, int user_id, int requested_by_id) {
    try {
        this->chatExistsAndNotPersonal(chat_id);

        if (user_id != requested_by_id) {
            this->ensureAdminChatMember(chat_id,requested_by_id);
        }

        this->ensureUserInChat(chat_id, requested_by_id);

        this->_repoChatMember->remove(chat_id, user_id);
        this->_log->info("Delete chat_member by user_id = {} from chat_id = {} "
                         "(requested_by_id = {} )", user_id, chat_id, requested_by_id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void ChatService::promoteToAdmin(int chat_id, int user_to_promote_id, int requested_by_id) {
    try {
        this->chatExistsAndNotPersonal(chat_id);
        this->ensureAdminChatMember(chat_id,requested_by_id);
        this->ensureUserInChat(chat_id, user_to_promote_id);

        this->_repoChatMember->promoteToAdmin(chat_id, user_to_promote_id);
        this->_log->info("User promoted to Admin by user_id = {} in chat_id = {}",
            user_to_promote_id, chat_id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void ChatService::removeChat(int chat_id, int requested_by_id) {
    try {
        auto chat = this->_repoChat->getById(chat_id);
        if (!chat.has_value()) {
            this->_log->debug("Not found chat by id = {}", chat_id);
            throw ChatNotFoundException(std::to_string(chat_id));
        }
        this->_log->info("Got chat by id = {}", chat_id);

        this->ensureAdminChatMember(chat_id, requested_by_id);

        this->_repoChat->remove(chat_id);
        this->_log->info("Deleted chat by id = {}", chat_id);
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

Chat ChatService::chatExistsAndNotPersonal(int chat_id) {
    auto chat = this->_repoChat->getById(chat_id);
    if (!chat.has_value()) {
        this->_log->debug("Not found chat by id = {}", chat_id);
        throw ChatNotFoundException(std::to_string(chat_id));
    }
    this->_log->info("Got chat by id = {}", chat_id);
    if (chat->type == PERSONAL) {
        this->_log->debug("Personal chat cannot be changed by chat_id = {}", chat_id);
        throw PersonalChatException(std::to_string(chat_id));
    }
    return chat.value();
}


void ChatService::ensureUserInChat(int chat_id, int user_id) {
    if (!this->_repoChatMember->exists(chat_id, user_id)) {
        this->_log->debug("Not found chatmember by user_id = {} and chat_id = {}",
                user_id, chat_id);
        throw ChatMemberNotFoundException(std::to_string(chat_id));
    }
    this->_log->info("Got chat member by chat_id = {} and user_id = {}", chat_id, user_id);
}


void ChatService::ensureAdminChatMember(int chat_id, int requested_by_id) {
    auto chat_member = this->_repoChatMember->getByChatAndUser(chat_id,requested_by_id);
    if (!chat_member.has_value()) {
        this->_log->debug("Not found chatmember by user_id = {} and chat_id = {}",
            requested_by_id, chat_id);
        throw ChatMemberNotFoundException(std::to_string(chat_id));
    }
    if (!chat_member->isAdmin()) {
        this->_log->debug("User is not Admin by user_id = {}", requested_by_id);
        throw PermissionException(std::to_string(requested_by_id));
    }
    this->_log->info( "Got Admin chat member by chat_id = {} and user_id = {}", chat_id, requested_by_id);
}
