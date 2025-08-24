#ifndef MESSAGE_H
#define MESSAGE_H

#include <soci/soci.h>
#include <soci/values.h>
#include <crow/json.h>

struct Message {
    int id;
    int chat_id;
    int user_id;
    std::string content;
    std::time_t edited_at;
    std::time_t created_at;

    crow::json::wvalue to_json() const {
            return {{"id", this->id},{"chat_id", this->chat_id},
            {"user_id", this->user_id}, {"content", this->content},
            {"edited_at", static_cast<int64_t>(this->edited_at)},
            {"created_at", static_cast<int64_t>(this->created_at)}};
        }

};

namespace soci {
    template<>
    struct type_conversion<Message> {
        typedef values base_type;

        static void from_base(values const &v, indicator /* ind */, Message &message) {
            message.id = v.get<int>("id");
            message.chat_id = v.get<int>("chat_id");
            message.user_id = v.get<int>("user_id");
            message.content = v.get<std::string>("content");

            auto created_tm = v.get<std::tm>("created_at");
            message.created_at = std::mktime(&created_tm);

            if (v.get_indicator("edited_at") == i_ok) {
                std::tm edited_tm = v.get<std::tm>("edited_at");
                message.edited_at = std::mktime(&edited_tm);
            } else {
                message.edited_at = 0;
            }
        }

        static void to_base(const Message &message, values &v, indicator &ind) {
            v.set("id", message.id);
            v.set("chat_id", message.chat_id);
            v.set("user_id", message.user_id);
            v.set("content", message.content);
            if (message.edited_at == 0) {
                v.set("edited_at", std::tm{}, i_null);
            } else {
                const std::tm edited_at = * std::localtime(&message.edited_at);
                v.set("edited_at", edited_at);
            }
            const std::tm created_at = * std::localtime(&message.created_at);
            v.set("created_at", created_at);
            ind = i_ok;
        }
    };
}

#endif //MESSAGE_H
