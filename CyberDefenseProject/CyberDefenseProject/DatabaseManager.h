#pragma once
#include "sqlite3.h"
#include <optional>
#include <string>
#include "IDatabase.h"
#include "SQLiteDatabase.h"


class DatabaseManager {
public:
    static DatabaseManager& getInstance();

    std::optional<std::string> getCountryByIP(const std::string& ip);
    bool saveGeo(const GeoRecord& rec);
    std::vector<std::string> getIPsByCountry(const std::string& country);
    std::vector<std::pair<std::string, int>> getTopCountries(int limit);

private:
    std::unique_ptr<IDatabase> m_db;
    DatabaseManager() : m_db(std::make_unique<SQLiteDatabase>("geo.db")) {} ;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};
