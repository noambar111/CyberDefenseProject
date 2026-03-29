#pragma once
#include <string>
#include <vector>
#include <optional>

struct GeoRecord {
    std::string ip;
    std::string country;
    std::string timestamp;
};

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual bool saveGeo(const GeoRecord& rec) = 0;
    virtual std::optional<std::string> getCountryByIP(const std::string& ip) = 0;
    virtual std::vector<std::string> getIPsByCountry(const std::string& country) = 0;
    virtual std::vector<std::pair<std::string, int>> getTopCountries(int limit) = 0;
};