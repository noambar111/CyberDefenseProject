#include "IPsByCountryHandler.h"
#include "ResponseBuilder.h"

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

    if (countryToIps.find(country) == countryToIps.end())
    {
        return ResponseBuilder()
        .setStatus(200)
        .setBody("[]")
        .build();
    }

    std::string body = "[";
    for (const auto& ip : countryToIps[country])
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
