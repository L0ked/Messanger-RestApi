#include "ChatMemberRepository.h"

std::optional<std::vector<Chat> > ChatMemberRepository::getChatsByUserId(int user_id) {
    try {
        const soci::rowset<Chat> rs = (this->_session.prepare
                                       << "select c.id, c.name, c.description, c.type, c.created_at from chat_members cm "
                                       "join chats c on cm.chat_id = c.id where cm.user_id = :user_id;", soci::use(
                                           user_id));
        std::vector<Chat> chats;
        for (auto &row: rs) {
            chats.push_back(row);
        }
        if (chats.empty())
            return std::nullopt;
        return chats;

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<std::vector<User> > ChatMemberRepository::getUserByChatId(int chat_id) {
    try {
        const soci::rowset<User> rs = (this->_session.prepare
                                             << "select u.id, u.username, u.display_name, u.password, u.created_at from chat_members cm "
                                             "join users u on cm.user_id = u.id where cm.chat_id = :chat_id;", soci::use(
                                                 chat_id));
        std::vector<User> users;
        for (auto &row: rs) {
            users.push_back(row);
        }
        if (users.empty())
            return std::nullopt;
        return users;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<ChatMember> ChatMemberRepository::getByChatAndUser(int chat_id, int user_id) {
    try {
        const soci::rowset<ChatMember> rs = (this->_session.prepare << "select * from chat_members where user_id = :user_id and chat_id = :chat_id;",
                soci::use(user_id), soci::use(chat_id));

        for (auto& row : rs) {
            return row;
        }
        return std::nullopt;

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

bool ChatMemberRepository::exists(int chat_id, int user_id) {
    try {
        int result;
        this->_session << "select exists (select 1 from chat_members where user_id = :user_id and chat_id = :chat_id);",
                soci::into(result),
                soci::use(user_id), soci::use(chat_id);
        return result == 1;
    } catch (const soci::soci_error &) {
        throw;
    }
}

void ChatMemberRepository::create(ChatMember &chat_member) {
    try {
        this->_session << "insert into chat_members(chat_id, user_id, role, created_at) "
                "values(:chat_id, :user_id, :role, :created_at);",
                soci::use(chat_member);
    } catch (const soci::soci_error &) {
        throw;
    }
}

void ChatMemberRepository::promoteToAdmin(int chat_id, int user_id) {
    try {
        this->_session << "update chat_members set role = 1 where chat_id = :chat_id and user_id = :user_id;",
            soci::use(chat_id), soci::use(user_id);
    } catch (const soci::soci_error &) {
        throw;
    }
}

void ChatMemberRepository::remove(int chat_id, int user_id) {
    try {
        this->_session << "delete from chat_members where chat_id = :chat_id and user_id = :user_id;",
                soci::use(chat_id), soci::use(user_id);
    } catch (const soci::soci_error &) {
        throw;
    }
}
