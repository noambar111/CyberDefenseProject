#include "Router.h"
#include "ResponseBuilder.h"

void Router::addRoute(const std::string& path, std::unique_ptr<IHandler> handler)
{
	m_routes[path] = std::move(handler);
}

Response Router::route(const Request& req)
{
	auto it = m_routes.find(req.path());
	if (it != m_routes.end())
	{
		return it->second->handle(req);
	}

	return ResponseBuilder()
		.setStatus(404)
		.setBody("Not Found")
		.build();
}
