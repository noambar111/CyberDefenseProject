#include "Request.h"
#include <sstream>


Request::Request(const std::string& raw)
{
    int requestLineEnd_idx = raw.find("\r\n");
    std::istringstream iss(raw.substr(0, requestLineEnd_idx));


    if (iss) iss >> this->m_method;
    if (iss) iss >> this->m_path;
    if (iss) iss >> this->m_version;

    if (m_path.size())
    {
        int paramIdxStart = m_path.find('?');

        if (paramIdxStart != std::string::npos)
        {
            std::istringstream queryParam(m_path.substr(paramIdxStart + 1));
            std::string kv;
            while (std::getline(queryParam, kv, '&'))
            {
                int eqIdx = kv.find('=');
                if (eqIdx != std::string::npos)
                {
                    std::string key = kv.substr(0, eqIdx);
                    std::string value = kv.substr(eqIdx + 1);
                    m_params[key] = value;
                }
            }
        }
        m_path = m_path.substr(0, paramIdxStart);
    }
    
}
