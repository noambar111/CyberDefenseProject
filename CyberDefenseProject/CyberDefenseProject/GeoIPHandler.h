#pragma once
#include "IHandler.h"

class GepIPHandler : public IHandler
{
public:
	Response handle(const Request& req) override;
};