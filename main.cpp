#include <crow.h>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <memory>

#include "logger/Logger.h"
#include "service/FactoryService.h"
#include "handler/FactoryHandler.h"
#include "middleware/AuthMiddleware.h"

int main() {
    auto logger = logger::create("main", spdlog::level::debug);

    try {
        soci::session sql("postgresql", "dbname=MessengerTest user=postgres password=Wowa+ host=localhost");

        auto serviceFactory = createServiceFactory(sql, logger);

        auto authService = serviceFactory->createAuthService();

        AuthMiddleware authMiddleware(authService);
        crow::App<AuthMiddleware> app(authMiddleware);
        // app.loglevel(crow::LogLevel::Critical);

        auto handlerFactory = std::make_unique<FactoryHandler>(std::move(serviceFactory));

        auto userHandler = handlerFactory->createUserHandler();
        auto authHandler = handlerFactory->createAuthHandler();
        auto chatHandler = handlerFactory->createChatHandler();
        auto messageHandler = handlerFactory->createMessageHandler();

        userHandler->registerRouters(app);
        authHandler->registerRouters(app);
        chatHandler->registerRouters(app);
        messageHandler->registerRouters(app);

        CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::OPTIONS)
        ([](const crow::request& req, const std::string& path) {
            crow::response res(200);
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return res;
        });

        logger->info("Starting server on port 8080");
        app.port(8080).multithreaded().run();

    } catch (const std::exception& e) {
        logger->error("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}