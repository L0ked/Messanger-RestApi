#include <crow.h>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <memory>

#include "logger/Logger.h"
#include "migrations/Migrator.h"
#include "service/FactoryService.h"
#include "handler/FactoryHandler.h"
#include "middleware/AuthMiddleware.h"

int main() {
    auto logger = logger::create("main", spdlog::level::debug);
    PasswordHasher::init();

    try {
        soci::session sql("postgresql", "dbname=MessengerTest user=postgres password=Wowa+ host=localhost");
        runMigrations(sql, "migrations", logger);

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

        logger->info("Starting server on port 8080");
        app.port(8080).multithreaded().run();

    } catch (const std::exception& e) {
        logger->error("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}