#pragma once
#include "Request.h"
#include "Response.h"

class IHandler
{
public:
    virtual Response handle(const Request& req) = 0;
    virtual ~IHandler() = default;
};