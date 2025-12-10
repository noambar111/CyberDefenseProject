#include "TopCountriesHandler.h"
#include "ResponseBuilder.h"
#include "DatabaseManager.h"

Response TopCountriesHandler::handle(const Request& req)
{
    int limit = 5;
    auto it = req.params().find("limit");
    if (it != req.params().end())
    {
        try
        {
            limit = std::stoi(it->second);
        }
        catch (...)
        {
            limit = 5;
        }
    }

    auto top = DatabaseManager::getInstance().getTopCountries(limit);
    std::string json = "{ \"top_countries\": [";

    for (const auto& [country, count] : top)
    {
        json += "{ \"country\": \"" + country + "\", \"count\": " + std::to_string(count) + " },";
    }


    if (!top.empty()) json.pop_back(); 

    json += "] }";
    

    return ResponseBuilder()
        .setStatus(200)
        .addHeader("Content-Type", "application/json")
        .setBody(json)
        .build();
}
