#ifndef CHAT_H
#define CHAT_H

#include <soci/soci.h>
#include <soci/values.h>
#include <crow/json.h>

enum ChatType {
    PERSONAL = 1,
    GROUPING = 2
};

struct Chat {
    int id;
    std::string name;
    std::string description;
    ChatType type;
    std::time_t created_at;

    crow::json::wvalue to_json() const {
        if (this->type == PERSONAL) {
            return {{"id", this->id},{"type", static_cast<int>(this->type)}};
        }
        return {{"id", this->id},{"type", static_cast<int>(this->type)},
            {"name" , this->name}, {"description", this->description}};
    }
};

namespace soci {
    template<>
    struct type_conversion<ChatType> {
        typedef int base_type;

        static void from_base(int i, indicator , ChatType &chatType) {
            chatType = static_cast<ChatType>(i);
        }

        static void to_base(const ChatType &chatType, int &i, indicator &ind) {
            i = static_cast<int>(chatType);
            ind = i_ok;
        }
    };

    template<>
    struct type_conversion<Chat> {
        typedef values base_type;

        static void from_base(values const &v, indicator /* ind */, Chat &chat) {
            chat.id = v.get<int>("id");
            chat.name = v.get<std::string>("name");
            chat.description = v.get<std::string>("description");
            chat.type = v.get<ChatType>("type");
            auto created_tm = v.get<std::tm>("created_at");
            chat.created_at = std::mktime(&created_tm);
        }

        static void to_base(const Chat &chat, values &v, indicator &ind) {
            v.set("id", chat.id);
            v.set("name", chat.name);
            v.set("description", chat.description);
            v.set("type", chat.type);
            const std::tm created_at = * std::localtime(&chat.created_at);
            v.set("created_at", created_at);
            ind = i_ok;
        }
    };
}

#endif //CHAT_H
