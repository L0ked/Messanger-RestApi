#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H


#include <crow.h>
#include <utility>

#include "service/AuthService.h"
#include "helpers/ResponseUtils.h"
#include "middleware/AuthMiddleware.h"

class AuthHandler {
    std::shared_ptr<AuthService> _service;
public:
    explicit AuthHandler(std::shared_ptr<AuthService> service):
        _service(std::move(service)) {}

    void registerRouters(crow::App<AuthMiddleware> &app);

    crow::response registerUser(const crow::request &req);
    crow::response login(const crow::request &req);
    crow::response logout(const crow::request &req);
    crow::response logoutAll(const crow::request &req);
};


#endif //AUTHHANDLER_H
