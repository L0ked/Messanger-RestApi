#ifndef AUTHMIDDLEWARE_H
#define AUTHMIDDLEWARE_H


#include <crow.h>
#include "context/AuthContext.h"
#include "service/AuthService.h"
#include "helpers/ResponseUtils.h"


class AuthMiddleware: crow::ILocalMiddleware {
    std::shared_ptr<AuthService> _authService;
public:
    using context = AuthContext;

    explicit AuthMiddleware(std::shared_ptr<AuthService> authService) :
            _authService(std::move(authService)) {}

    void before_handle(crow::request& req, crow::response& res, context& ctx);

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};

#endif //AUTHMIDDLEWARE_H
