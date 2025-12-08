#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager inst;
    return inst;
}

std::optional<std::string> DatabaseManager::getCountryByIP(const std::string& ip)
{
    this->m_db.getCountryByIP(ip);
}

bool DatabaseManager::saveGeo(const GeoRecord& rec)
{
    return false;
}

std::vector<std::string> DatabaseManager::getIPsByCountry(const std::string& country)
{
    return std::vector<std::string>();
}

std::vector<std::pair<std::string, int>> DatabaseManager::getTopCountries()
{
    return std::vector<std::pair<std::string, int>>();
}
