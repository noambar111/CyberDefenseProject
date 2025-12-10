#include "IPsByCountryHandler.h"
#include "ResponseBuilder.h"
#include "DatabaseManager.h"

Response IPsByCountryHandler::handle(const Request& req)
{
    auto it = req.params().find("country");
    if (it == req.params().end())
    {
        return ResponseBuilder()
            .setStatus(200)
            .setBody("Missing country parameter")
            .build();
    }

    const std::string& country = it->second;
    auto ips = DatabaseManager::getInstance().getIPsByCountry(country);


    if (!ips.size())
    {
        return ResponseBuilder()
        .setStatus(200)
        .setBody("[]")
        .build();
    }

    std::string body = "[";
    for (const auto& ip : ips)
    {
        body += "\"" + ip + "\",";
    }

    body.pop_back();

    body += "]";

    return ResponseBuilder()
        .setStatus(200)
        .setBody(body)
        .build();
}
