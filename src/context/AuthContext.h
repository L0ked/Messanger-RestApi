#ifndef AUTHCONTEXT_H
#define AUTHCONTEXT_H

#include <optional>

struct AuthContext {
    std::optional<int> user_id;
};

#endif //AUTHCONTEXT_H
