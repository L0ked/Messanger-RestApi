#ifndef REPOSITORYFACTORY_H
#define REPOSITORYFACTORY_H

#include <memory>

#include "repository/IUserRepository.h"
#include "repository/ISessionRepository.h"
#include "repository/IChatRepository.h"
#include "repository/IChatMemberRepository.h"
#include "repository/IMessageRepository.h"

#include "repository/postgres/UserRepository.h"
#include "repository/postgres/SessionRepository.h"
#include "repository/postgres/ChatRepository.h"
#include "repository/postgres/ChatMemberRepository.h"
#include "repository/postgres/MessageRepository.h"

class FactoryRepository {
public:
    virtual ~FactoryRepository() = default;

    virtual std::shared_ptr<IUserRepository> createUserRepository() = 0;
    virtual std::shared_ptr<ISessionRepository> createSessionRepository() = 0;
    virtual std::shared_ptr<IChatRepository> createChatRepository() = 0;
    virtual std::shared_ptr<IChatMemberRepository> createChatMemberRepository() = 0;
    virtual std::shared_ptr<IMessageRepository> createMessageRepository() = 0;
};



class FactoryRepositoryPostgres final : public FactoryRepository {
    soci::session& _session;

    std::shared_ptr<IUserRepository> _userRepository;
    std::shared_ptr<ISessionRepository> _sessionRepository;
    std::shared_ptr<IChatRepository> _chatRepository;
    std::shared_ptr<IChatMemberRepository> _chatMemberRepository;
    std::shared_ptr<IMessageRepository> _messageRepository;
public:
    explicit FactoryRepositoryPostgres(soci::session& session)
        : _session(session) {}

    std::shared_ptr<IUserRepository> createUserRepository() override {
        if (!this->_userRepository) {
            this->_userRepository = std::make_shared<UserRepository>(this->_session);
        }
        return this->_userRepository;
    }

    std::shared_ptr<ISessionRepository> createSessionRepository() override {
        if (!this->_sessionRepository) {
            this->_sessionRepository = std::make_shared<SessionRepository>(this->_session);
        }
        return this->_sessionRepository;
    }

    std::shared_ptr<IChatRepository> createChatRepository() override {
        if (!this->_chatRepository) {
            this->_chatRepository = std::make_shared<ChatRepository>(this->_session);
        }
        return this->_chatRepository;
    }

    std::shared_ptr<IChatMemberRepository> createChatMemberRepository() override {
        if (!this->_chatMemberRepository) {
            this->_chatMemberRepository = std::make_shared<ChatMemberRepository>(this->_session);
        }
        return this->_chatMemberRepository;
    }

    std::shared_ptr<IMessageRepository> createMessageRepository() override {
        if (!this->_messageRepository) {
            this->_messageRepository = std::make_shared<MessageRepository>(this->_session);
        }
        return this->_messageRepository;
    }
};



class FactoryRepositoryProvider {
public:
    static std::unique_ptr<FactoryRepository> createFactory(soci::session& session) {
        return std::make_unique<FactoryRepositoryPostgres>(session);
    }
};

#endif //REPOSITORYFACTORY_H
