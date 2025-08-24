#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace logger {
    std::shared_ptr<spdlog::logger> create(
        const std::string& name = "app",
        spdlog::level::level_enum level = spdlog::level::info
    );

}

#endif // LOGGER_H
