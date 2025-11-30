#pragma once
#include "IHandler.h"
#include <unordered_map>

class IPsByCountryHandler : public IHandler
{
public:
    IPsByCountryHandler()
    {
        countryToIps["Israel"] = { "1.1.1.1", "5.5.5.5" };
        countryToIps["USA"] = { "8.8.8.8", "7.7.7.7", "9.9.9.9" };
        countryToIps["Germany"] = { "4.4.4.4" };
    }
    Response handle(const Request& req) override;
private:
    std::unordered_map<std::string, std::vector<std::string>> countryToIps;
};