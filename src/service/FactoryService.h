#ifndef FACTORYSERVICE_H
#define FACTORYSERVICE_H


#include "service/UserService.h"
#include "service/AuthService.h"
#include "service/ChatService.h"
#include "service/MessageService.h"
#include "logger/Logger.h"

#include "repository/FactoryRepository.h"

class FactoryService {
    std::unique_ptr<FactoryRepository> _factoryRepository;

    std::shared_ptr<UserService> _userService;
    std::shared_ptr<AuthService> _authService;
    std::shared_ptr<ChatService> _chatService;
    std::shared_ptr<MessageService> _messageService;
    std::shared_ptr<spdlog::logger> _logger;
public:
    FactoryService(
        std::unique_ptr<FactoryRepository> factoryRepository,
        std::shared_ptr<spdlog::logger> logger) :
    _factoryRepository(std::move(factoryRepository)),
    _logger(std::move(logger)) {}

    std::shared_ptr<UserService> createUserService() {
        if (!this->_userService) {
            auto userRepo = this->_factoryRepository->createUserRepository();
            this->_userService = std::make_shared<UserService>(userRepo, this->_logger);
        }
        return this->_userService;
    }

    std::shared_ptr<AuthService> createAuthService() {
        if (!this->_authService) {
            auto sessionRepo = this->_factoryRepository->createSessionRepository();
            auto userService = this->createUserService();
            this->_authService = std::make_shared<AuthService>(sessionRepo, userService, this->_logger);
        }
        return this->_authService;
    }

    std::shared_ptr<ChatService> createChatService() {
        if (!this->_chatService) {
            auto chatRepo = this->_factoryRepository->createChatRepository();
            auto chatMemberRepo = this->_factoryRepository->createChatMemberRepository();
            this->_chatService = std::make_shared<ChatService>(chatRepo, chatMemberRepo, this->_logger);
        }
        return this->_chatService;
    }

    std::shared_ptr<MessageService> createMessageService() {
        if (!this->_messageService) {
            auto messageRepo = this->_factoryRepository->createMessageRepository();
            auto chatService = this->createChatService();
            this->_messageService = std::make_shared<MessageService>(messageRepo, chatService, this->_logger);
        }
        return this->_messageService;
    }
};



inline std::unique_ptr<FactoryService> createServiceFactory(soci::session& session, std::shared_ptr<spdlog::logger> logger = nullptr) {
    if (!logger) {
        logger = logger::create("service", spdlog::level::info);
    }
    auto repositoryFactory = FactoryRepositoryProvider::createFactory(session);
    return std::make_unique<FactoryService>(
        std::move(repositoryFactory),
        logger
    );
}


#endif //FACTORYSERVICE_H
