#include "ChatRepository.h"

std::optional<Chat> ChatRepository::getById(int id) {
    try {
        const soci::rowset<Chat> rs = (this->_session.prepare << "select * from chats where id = :id",
                soci::use(id));

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

std::optional<std::vector<Chat> > ChatRepository::getByName(std::string chat_name) {
    try {
        const soci::rowset<Chat> rs = (_session.prepare
                                 << "select * from chats where name = :chat_name limit 15", soci::use(chat_name));
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

std::optional<Chat> ChatRepository::findPersonalChat(int user_id1, int user_id2) {
    try {
        const soci::rowset<Chat> rs = (this->_session.prepare << "select ch.id, ch.name, ch.description, ch.type, ch.created_at "
                "from chats ch "
                "join chat_members cm1 on ch.id = cm1.chat_id "
                "join chat_members cm2 on ch.id = cm2.chat_id "
                "where ch.type = 1 AND cm1.user_id = :user_id1 AND cm2.user_id = :user_id2 "
                "LIMIT 1",
                soci::use(user_id1), soci::use(user_id2));
        for (const auto& row : rs) {
            return row;
        }
        return std::nullopt;

    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

int ChatRepository::create(Chat &chat) {
    try {
        int id;
        this->_session << "insert into chats(name, description, type, created_at) "
                "values(:name, :description, :type, :created_at) returning id",
                soci::use(chat), soci::into(id);
        return id;
    } catch (const soci::soci_error & ) {
        throw;
    }
}

int ChatRepository::createChatAndChatMembers(Chat &chat, std::vector<ChatMember> &chat_members) {
    soci::transaction tx(this->_session);
    try {
        int id;
        this->_session << "insert into chats(name, description, type, created_at) "
                "values(:name, :description, :type, :created_at) returning id",
                soci::use(chat), soci::into(id);
        for (auto& i: chat_members) {
            i.chat_id = id;
            this->_session << "insert into chat_members(chat_id, user_id, role, created_at) "
                "values(:chat_id, :user_id, :role, :created_at)",
                soci::use(i);
        }
        tx.commit();
        return id;
    } catch (const soci::soci_error &ex) {
        tx.rollback();
        throw;
    }
}

void ChatRepository::update(Chat &chat) {
    try {
        this->_session << "update chats set name = :name, description = :description "
                "where id = :id", soci::use(chat);
    } catch (const soci::soci_error &) {
        throw;
    }
}

void ChatRepository::remove(int id) {
    try {
        this->_session << "delete from chats where id = :id", soci::use(id);
    } catch (const soci::soci_error &) {
        throw;
    }
}
