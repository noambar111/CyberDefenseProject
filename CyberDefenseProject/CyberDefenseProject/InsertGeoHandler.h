#pragma once
#include "IHandler.h"

class InsertGeoHandler : public IHandler
{
public:
	Response handle(const Request& req) override;
};
