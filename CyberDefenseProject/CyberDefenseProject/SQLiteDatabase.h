#pragma once
#include "IDatabase.h"
#include "sqlite3.h"
#include "Logger.h"

class SQLiteDatabase : public IDatabase {
public:
    SQLiteDatabase(const std::string& fileName);
    ~SQLiteDatabase();

    bool saveGeo(const GeoRecord& rec) override;
    std::optional<std::string> getCountryByIP(const std::string& ip) override;
    std::vector<std::string> getIPsByCountry(const std::string& country) override;
    std::vector<std::pair<std::string, int>> getTopCountries(int limit) override;

private:
    sqlite3* m_db;
    void initSchema();
};