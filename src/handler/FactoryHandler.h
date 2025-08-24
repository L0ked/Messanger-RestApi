#ifndef FACTORYHANDLER_H
#define FACTORYHANDLER_H

#include "UserHandler.h"
#include "AuthHandler.h"
#include "ChatHandler.h"
#include "MessageHandler.h"

#include "service/FactoryService.h"

class FactoryHandler {
    std::unique_ptr<FactoryService> _factoryService;

    std::shared_ptr<UserHandler> _userHandler;
    std::shared_ptr<AuthHandler> _authHandler;
    std::shared_ptr<ChatHandler> _chatHandler;
    std::shared_ptr<MessageHandler> _messageHandler;
public:
    explicit FactoryHandler(std::unique_ptr<FactoryService> factoryService) :
    _factoryService(std::move(factoryService)) {}

    std::shared_ptr<UserHandler> createUserHandler() {
        if (!this->_userHandler) {
            auto userService = this->_factoryService->createUserService();
            this->_userHandler = std::make_shared<UserHandler>(userService);
        }
        return this->_userHandler;
    }

    std::shared_ptr<AuthHandler> createAuthHandler() {
        if (!this->_authHandler) {
            auto authService = this->_factoryService->createAuthService();
            this->_authHandler = std::make_shared<AuthHandler>(authService);
        }
        return this->_authHandler;
    }

    std::shared_ptr<ChatHandler> createChatHandler() {
        if (!this->_chatHandler) {
            auto chatService = this->_factoryService->createChatService();
            this->_chatHandler = std::make_shared<ChatHandler>(chatService);
        }
        return this->_chatHandler;
    }

    std::shared_ptr<MessageHandler> createMessageHandler() {
        if (!this->_messageHandler) {
            auto messageService = this->_factoryService->createMessageService();
            this->_messageHandler = std::make_shared<MessageHandler>(messageService);
        }
        return this->_messageHandler;
    }
};



#endif //FACTORYHANDLER_H
