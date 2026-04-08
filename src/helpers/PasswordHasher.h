// src/helpers/PasswordHasher.h
#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H

#include <sodium.h>
#include <string>
#include <stdexcept>

class PasswordHasher {
public:
    static void init() {
        if (sodium_init() < 0)
            throw std::runtime_error("sodium_init failed");
    }

    static std::string hash(const std::string& password) {
        char out[crypto_pwhash_STRBYTES];
        if (crypto_pwhash_str(
                out,
                password.c_str(),
                password.size(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            throw std::runtime_error("crypto_pwhash_str failed (out of memory?)");
                }
        return std::string(out);
    }

    static bool verify(const std::string& password, const std::string& stored) {
        return crypto_pwhash_str_verify(
                   stored.c_str(),
                   password.c_str(),
                   password.size()) == 0;
    }
};

#endif