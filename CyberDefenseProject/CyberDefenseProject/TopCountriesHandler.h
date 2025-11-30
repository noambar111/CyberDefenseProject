#pragma once
#include "IHandler.h"

class TopCountriesHandler : public IHandler
{
public:
    Response handle(const Request& req) override;
};
