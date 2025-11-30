#include "GeoIPHandler.h"
#include "ResponseBuilder.h"

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
    std::string country;

    if (ip == "1.1.1.1") country = "US";
    else if (ip == "8.8.8.8") country = "Global";
    else country = "Unknown";

    std::string json = "{ \"ip\": \"" + ip + "\", \"country\": \"" + country + "\" }";

    return  ResponseBuilder()
        .setStatus(200)
        .addHeader("Content-Type", "application/json")
        .setBody(json)
        .build();
}
