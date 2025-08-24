#ifndef CHATMEMBER_H
#define CHATMEMBER_H

#include <soci/soci.h>
#include <soci/values.h>

enum ChatRole {
    MEMBER = 0,
    ADMIN = 1
};

struct ChatMember {
    int chat_id;
    int user_id;
    ChatRole role;
    std::time_t created_at;

    bool isAdmin() const {
        return this->role == ADMIN;
    }
};


namespace soci {
    template<>
       struct type_conversion<ChatRole> {
        typedef int base_type;

        static void from_base(int i, indicator , ChatRole &chatRole) {
            chatRole = static_cast<ChatRole>(i);
        }

        static void to_base(const ChatRole &chatRole, int &i, indicator &ind) {
            i = static_cast<int>(chatRole);
            ind = i_ok;
        }
    };

    template<>
    struct type_conversion<ChatMember> {
        typedef values base_type;

        static void from_base(values const &v, indicator /* ind */, ChatMember &chat_member) {
            chat_member.chat_id = v.get<int>("chat_id");
            chat_member.user_id = v.get<int>("user_id");
            chat_member.role = v.get<ChatRole>("role");
            auto created_tm = v.get<std::tm>("created_at");
            chat_member.created_at = std::mktime(&created_tm);
        }

        static void to_base(const ChatMember &chat_member, values &v, indicator &ind) {
            v.set("chat_id", chat_member.chat_id);
            v.set("user_id", chat_member.user_id);
            v.set("role", chat_member.role);
            const std::tm created_at = * std::localtime(&chat_member.created_at);
            v.set("created_at", created_at);
            ind = i_ok;
        }
    };
}

#endif //CHATMEMBER_H
