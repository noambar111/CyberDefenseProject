#include "TopCountriesHandler.h"
#include "ResponseBuilder.h"

Response TopCountriesHandler::handle(const Request& req)
{
    std::string json =
        R"({
            "top_countries": [
                { "country": "USA", "count": 120 },
                { "country": "Germany", "count": 85 },
                { "country": "Israel", "count": 52 },
                { "country": "France", "count": 30 },
                { "country": "UK", "count": 21 }
            ]
        })";
    return ResponseBuilder()
        .setStatus(200)
        .addHeader("Content-Type", "application/json")
        .setBody(json)
        .build();
}
