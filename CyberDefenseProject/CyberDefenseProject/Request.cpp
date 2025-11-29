#include "Request.h"
#include <sstream>


Request::Request(const std::string& raw)
{
    int requestLineEnd_idx = raw.find("\r\n");

    std::istringstream iss(raw.substr(0, requestLineEnd_idx));

    std::string method, path, version;
    std::map<std::string, std::string> p_map;


    if (iss) iss >> method;
    if (iss) iss >> path;
    if (iss) iss >> version;

    if (path.size())
    {
        int paramIdxStart = path.find('?');

        if (paramIdxStart != std::string::npos)
        {
            std::istringstream queryParam(path.substr(paramIdxStart + 1));
            std::string kv;
            while (std::getline(queryParam, kv, '&'))
            {
                int eqIdx = kv.find('=');
                if (eqIdx != std::string::npos)
                {
                    std::string key = kv.substr(0, eqIdx);
                    std::string value = kv.substr(eqIdx + 1);
                    p_map[key] = value;
                }
            }
        }
    }
}
