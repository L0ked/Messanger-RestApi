#ifndef USERHANDLER_H
#define USERHANDLER_H

#include <crow.h>
#include <crow/app.h>
#include <crow/routing.h>
#include <crow/middleware.h>

#include <utility>
#include "service/UserService.h"
#include "helpers/ResponseUtils.h"
#include "exceptions/service_exceptions.h"
#include "middleware/AuthMiddleware.h"
#include "context/AuthContext.h"

class UserHandler {
    std::shared_ptr<UserService> _service;
public:
    explicit UserHandler(std::shared_ptr<UserService> service):
        _service(std::move(service)) {}

    void registerRouters(crow::App<AuthMiddleware> & app);

    crow::response getById(const crow::request & req, int id);
    crow::response getByUsername(const crow::request & req, std::string username);

    crow::response create(const crow::request & req);
    crow::response change_password(const crow::request & req);
    crow::response update(const crow::request & req);
    crow::response remove_self(const crow::request & req);
};


#endif //USERHANDLER_H
