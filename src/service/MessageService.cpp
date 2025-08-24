#include "MessageService.h"

std::optional<Message> MessageService::getById(int id, int requested_by_id) {
    try {
        auto message = this->_repository->getById(id);
        if (!message.has_value())
            this->_log->debug("Not found message by id = {}", id);
        else {
            this->_log->info("Got message by id = {}", id);
            this->_serviceChat->ensureUserInChat(message.value().chat_id, requested_by_id);
        }
        return message;
        
    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<std::vector<Message>> MessageService::getByChatId(int chat_id,
int limit, int offset, int requested_by_id) {
    try {
        this->_serviceChat->ensureUserInChat(chat_id, requested_by_id);

        auto messages = this->_repository->getByChatId(chat_id, limit, offset);
        if (!messages.has_value())
            this->_log->debug("Not found messages by chat_id = {}", chat_id);
        else
            this->_log->info("Got messages by chat_id = {}", chat_id);
        return messages;

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<std::vector<Message>> MessageService::getMessagesAfter(int chat_id, int message_id, int requested_by_id) {
    try {
        this->_serviceChat->ensureUserInChat(chat_id, requested_by_id);

        auto messages = this->_repository->getMessagesAfter(chat_id, message_id);
        if (!messages.has_value())
            this->_log->debug("Not found messages by chat_id = {} after message_id = {}", chat_id, message_id);
        else
            this->_log->info("Got messages by chat_id = {} after message_id = {}", chat_id, message_id);
        return messages;

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<std::vector<Message>> MessageService::getMessagesBefore(int chat_id, int message_id, int limit,
    int requested_by_id) {
    try {
        this->_serviceChat->ensureUserInChat(chat_id, requested_by_id);

        auto messages = this->_repository->getMessagesBefore(chat_id, message_id, limit);
        if (!messages.has_value())
            this->_log->debug("Not found messages by chat_id = {} before message_id = {}", chat_id, message_id);
        else
            this->_log->info("Got messages by chat_id = {} before message_id = {}", chat_id, message_id);
        return messages;

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

std::optional<Message> MessageService::getLastMessageInChat(int chat_id, int requested_by_id) {
    try {
        this->_serviceChat->ensureUserInChat(chat_id, requested_by_id);

        auto message = this->_repository->getLastMessageInChat(chat_id);
        if (!message.has_value())
            this->_log->debug("Not found last message by chat_id = {}", chat_id);
        else
            this->_log->info("Got last message by chat_id = {}", chat_id);
        return message;

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

Message MessageService::send(int chat_id, int requested_by_id, std::string content) {
    try {
        this->_serviceChat->ensureUserInChat(chat_id, requested_by_id);

        Message message(0, chat_id, requested_by_id, content, 0, std::time(nullptr));
        auto id = this->_repository->create(message);
        this->_log->info("Create message with id = {}", id);
        message.id = id;
        return message;

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void MessageService::edit(int id, std::string content, int requested_by_id) {
    try {
        auto message = this->ensureMessageOwnerOrAdmin(id, requested_by_id);

        message.content = content;
        message.edited_at = std::time(nullptr);
        this->_repository->update(message);
        this->_log->info("Edit message by id = {}", id);

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

void MessageService::remove(int id, int requested_by_id) {
    try {
        this->ensureMessageOwnerOrAdmin(id, requested_by_id);

        this->_repository->remove(id);
        this->_log->info("Deleted message by id = {}", id);

    } catch (const soci::soci_error &ex) {
        this->_log->error("Error Database: {}", ex.what());
        throw;
    }
}

Message MessageService::ensureMessageOwnerOrAdmin(int id, int requested_by_id) {
    auto message = this->_repository->getById(id);
    if (!message.has_value()) {
        this->_log->debug("Not found message by id = {}", id);
        throw MessageNotFoundException(std::to_string(id));
    }
    this->_log->info("Got message by id = {}", id);
    if (message.value().user_id != requested_by_id)
        this->_serviceChat->ensureAdminChatMember(message->chat_id, requested_by_id);
    return message.value();
}
