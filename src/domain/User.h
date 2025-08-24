#ifndef USER_H
#define USER_H


#include <soci/soci.h>
#include <soci/values.h>
#include <crow/json.h>

struct User {
    int id;
    std::string username;
    std::string password;
    std::string display_name;
    std::time_t created_at;

   crow::json::wvalue to_json() const {
       return {{"id", this->id},{"username", this->username},
       {"display_name", this->display_name}};
   }
};

namespace soci {
    template<>
    struct type_conversion<User> {
        typedef values base_type;

        static void from_base(values const &v, indicator /* ind */, User &user) {
            user.id = v.get<int>("id");
            user.username = v.get<std::string>("username");
            user.password = v.get<std::string>("password");
            user.display_name = v.get<std::string>("display_name");
            auto created_tm = v.get<std::tm>("created_at");
            user.created_at = std::mktime(&created_tm);
        }

        static void to_base(const User &user, values &v, indicator &ind) {
            v.set("id", user.id);
            v.set("username", user.username);
            v.set("password", user.password);
            v.set("display_name", user.display_name);
            const std::tm created_at = * std::localtime(&user.created_at);
            v.set("created_at", created_at);
            ind = i_ok;
        }
    };
}




#endif //USER_H
