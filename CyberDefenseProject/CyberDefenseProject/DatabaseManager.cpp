#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager inst;
    return inst;
}

std::optional<std::string> DatabaseManager::getCountryByIP(const std::string& ip)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return this->m_db->getCountryByIP(ip);
}

bool DatabaseManager::saveGeo(const GeoRecord& rec)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return this->m_db->saveGeo(rec);
}

std::vector<std::string> DatabaseManager::getIPsByCountry(const std::string& country)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return this->m_db->getIPsByCountry(country);
}

std::vector<std::pair<std::string, int>> DatabaseManager::getTopCountries(int limit)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return this->m_db->getTopCountries(limit);
}
