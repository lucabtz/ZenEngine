#include "URL.h"

#include <regex>

namespace ZenEngine
{
    Result<URL, std::string> ParseURL(const std::string &inURLString)
    {
        std::regex urlRegex(R"((\w+):\/\/(?:([^:]+)(?::([^@]+))?@)?(.+))");
        std::smatch match;
        if (std::regex_match(inURLString, match, urlRegex))
        {
            return URL(match[1], match[2], match[3], match[4]);
        }
        return std::string("Invalid URL");
    }

}