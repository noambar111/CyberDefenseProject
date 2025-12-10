#include "GeoIPHandler.h"
#include "ResponseBuilder.h"
#include "DatabaseManager.h"

Response GepIPHandler::handle(const Request& req)
{
    auto it = req.params().find("ip");
    if (it == req.params().end())
    {
        return ResponseBuilder()
            .setStatus(404)
            .setBody("Missing ip parameter")
            .build();
    }

    std::string ip = it->second;
    auto country = DatabaseManager::getInstance().getCountryByIP(ip);

    std::string countryStr = country.value_or("Unknown");

    std::string json = "{ \"ip\": \"" + ip + "\", \"country\": \"" + countryStr + "\" }";

    return  ResponseBuilder()
        .setStatus(200)
        .addHeader("Content-Type", "application/json")
        .setBody(json)
        .build();
}
