#ifndef RESPONSEUTILS_H
#define RESPONSEUTILS_H

#include <crow.h>
#include "exceptions/exceptions.h"

inline crow::response error_response(int status_code, const std::string& message) {
    crow::json::wvalue body;
    body["error"] = message;
    return crow::response(status_code, body);
}

inline crow::response error_response(int status_code, const AppException& ex) {
    return error_response(status_code, ex.what());
}

inline void error_response(int status_code, const std::string& message, crow::response & res) {
    res.code = status_code;
    crow::json::wvalue body;
    body["error"] = message;
    res.write(body.dump());
    res.end();
}
#endif //RESPONSEUTILS_H
