#pragma once
#include "IHandler.h"
#include <unordered_map>

class IPsByCountryHandler : public IHandler
{
public:
    Response handle(const Request& req) override;
};