#ifndef SESSION_H
#define SESSION_H

#include <soci/soci.h>
#include <soci/values.h>

struct Session {
    int id;
    int user_id;
    std::string token;
    std::time_t created_at;
};


namespace soci {
    template<>
    struct type_conversion<Session> {
        typedef values base_type;

        static void from_base(values const &v, indicator /* ind */, Session &session) {
            session.id = v.get<int>("id");
            session.user_id = v.get<int>("user_id");
            session.token = v.get<std::string>("token");
            auto created_tm = v.get<std::tm>("created_at");
            session.created_at = std::mktime(&created_tm);
        }

        static void to_base(const Session &session, values &v, indicator &ind) {
            v.set("id", session.id);
            v.set("user_id", session.user_id);
            v.set("token", session.token);
            const std::tm created_at = * std::localtime(&session.created_at);
            v.set("created_at", created_at);
            ind = i_ok;
        }
    };
}
#endif //SESSION_H
