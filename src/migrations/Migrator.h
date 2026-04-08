#ifndef MIGRATOR_H
#define MIGRATOR_H

#include <soci/soci.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <set>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

inline void runMigrations(soci::session& sql, const std::string& dir,
                          std::shared_ptr<spdlog::logger> log) {
    sql << "create table if not exists schema_migrations ("
           "version varchar(255) primary key, "
           "applied_at timestamptz default current_timestamp)";

    std::set<std::string> applied;
    soci::rowset<std::string> rs = (sql.prepare << "select version from schema_migrations");
    for (const auto& v : rs) applied.insert(v);

    std::vector<fs::path> files;
    for (const auto& e : fs::directory_iterator(dir))
        if (e.path().extension() == ".sql") files.push_back(e.path());
    std::sort(files.begin(), files.end());

    for (const auto& file : files) {
        std::string version = file.stem().string(); // "001_init"
        if (applied.contains(version)) {
            log->debug("Migration {} already applied, skipping", version);
            continue;
        }

        std::ifstream f(file);
        std::stringstream ss; ss << f.rdbuf();
        std::string content = ss.str();

        log->info("Applying migration {}", version);
        soci::transaction tx(sql);
        try {
            sql << content;
            sql << "insert into schema_migrations(version) values(:v)", soci::use(version);
            tx.commit();
            log->info("Migration {} applied", version);
        } catch (const std::exception& ex) {
            tx.rollback();
            log->error("Migration {} failed: {}", version, ex.what());
            throw;

        }
    }
}

#endif