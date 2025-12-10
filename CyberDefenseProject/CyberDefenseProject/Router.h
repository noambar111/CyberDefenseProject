#pragma once
#include <unordered_map>
#include <memory>
#include "Request.h"
#include "Response.h"
#include "IHandler.h"

class Router
{
public:
    Router();
    void addRoute(const std::string& path, std::unique_ptr<IHandler> handler);
    Response route(const Request& req);

private:
    std::unordered_map<std::string, std::unique_ptr<IHandler>> m_routes;
};