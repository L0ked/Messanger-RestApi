#include "AuthMiddleware.h"

void AuthMiddleware::before_handle(crow::request &req, crow::response &res, context &ctx) {

    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

    std::vector<std::string> public_paths = {
        "/register",
        "/login",
        "/users",
        "/logout",
        "/logout-all"
    };

    std::string path = req.url;
    for (const auto& public_path : public_paths) {
        if (path.starts_with(public_path)) {
            return;
        }
    }

    const auto auth_header = req.get_header_value("Authorization");

    if (auth_header.empty() || !auth_header.starts_with("Bearer ")) {
        error_response(401, "Missing or malformed Authorization header", res);
        return;
    }

    std::string token = auth_header.substr(7);

    try {
        auto user_id_opt = this->_authService->validateToken(token);
        if (user_id_opt == std::nullopt) {
            error_response(401, "Invalid token", res);
            return;
        }

        ctx.user_id = *user_id_opt;

    } catch (...) {
        error_response(500, "Internal error",res);
    }
}
