#include "MessageRepository.h"

std::optional<Message> MessageRepository::getById(int id) {
    try {
        const soci::rowset<Message> rs = (this->_session.prepare << "select * from messages where id = :id",
                soci::use(id));
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

std::optional<std::vector<Message>> MessageRepository::getByChatId(int chat_id, int limit, int offset) {
    try {
        const soci::rowset<Message> rs = (this->_session.prepare
                                       << "select * from messages where chat_id = :chat_id order by created_at desc "
                                          "limit :limit offset :offset",
                                       soci::use(chat_id), soci::use(limit), soci::use(offset));
        std::vector<Message> messages;
        for (const auto &row: rs) {
            messages.push_back(row);
        }
        if (messages.empty())
            return std::nullopt;
        return messages;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<std::vector<Message>> MessageRepository::getMessagesAfter(int chat_id, int message_id) {
    try {
        const soci::rowset<Message> rs = (this->_session.prepare
                                       << "select * from messages where chat_id = :chat_id and id > :message_id "
                                       "order by created_at asc",
                                       soci::use(chat_id), soci::use(message_id));
        std::vector<Message> messages;
        for (const auto &row: rs) {
            messages.push_back(row);
        }
        if (messages.empty())
            return std::nullopt;
        return messages;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<std::vector<Message>> MessageRepository::getMessagesBefore(int chat_id, int message_id, int limit) {
    try {
        const soci::rowset<Message> rs = (this->_session.prepare
                                       << "select * from messages where chat_id = :chat_id and id < :message_id "
                                       "order by created_at desc limit :limit",
                                       soci::use(chat_id), soci::use(message_id), soci::use(limit));
        std::vector<Message> messages;
        for (const auto &row: rs) {
            messages.push_back(row);
        }
        if (messages.empty())
            return std::nullopt;
        return messages;
    } catch (const soci::soci_error &ex) {
        if (ex.get_error_category() == soci::soci_error::no_data)
            return std::nullopt;
        throw;
    }
}

std::optional<Message> MessageRepository::getLastMessageInChat(int chat_id) {
    try {
        const soci::rowset<Message> rs = (this->_session.prepare << "select * from messages where chat_id = :chat_id order by created_at desc limit 1",
                soci::use(chat_id));
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

int MessageRepository::create(Message &message) {
    try {
        int id;
        this->_session << "insert into messages(chat_id, user_id, content, edited_at, created_at) "
                        "values(:chat_id, :user_id, :content, :edited_at, :created_at) returning id",
                        soci::use(message), soci::into(id);
        return id;
    } catch (const soci::soci_error &) {
        throw;
    }
}

void MessageRepository::update(Message &message) {
    try {
      this->_session << "update messages set content = :content, edited_at = :edited_at "
                        "where id = :id", soci::use(message);
    } catch (const soci::soci_error &) {
        throw;
    }
}

void MessageRepository::remove(int id) {
    try {
        this->_session << "delete from messages where id = :id", soci::use(id);
    } catch (const soci::soci_error &) {
        throw;
    }
}
