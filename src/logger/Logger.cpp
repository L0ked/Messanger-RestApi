#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace logger {
    std::shared_ptr<spdlog::logger> create(const std::string &name, spdlog::level::level_enum level) {
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>(name, console);
        logger->set_level(level);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] [%t] %v");
        return logger;
    }
}
