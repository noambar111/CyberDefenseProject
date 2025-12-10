#include "InsertGeoHandler.h"
#include "DatabaseManager.h"
#include "ResponseBuilder.h"
#include "Logger.h"

Response InsertGeoHandler::handle(const Request& req)
{
    auto& params = req.params();
    if (params.find("ip") == params.end() || params.find("country") == params.end())
    {
        return ResponseBuilder()
            .setStatus(400)
            .setBody("{\"error\": \"Missing ip or country parameters\"}")
            .addHeader("Content-Type", "application/json")
            .build();
    }

    GeoRecord rec{ params.at("ip"), params.at("country") };
    bool ok = DatabaseManager::getInstance().saveGeo(rec);

    if (!ok)
    {
        return  ResponseBuilder()
            .setStatus(500)
            .setBody("{\"error\": \"Failed to insert record\"}")
            .addHeader("Content-Type", "application/json")
            .build();
    }

    return ResponseBuilder()
        .setStatus(200)
        .setBody("{\"status\": \"record inserted\"}")
        .addHeader("Content-Type", "application/json")
        .build();

}
