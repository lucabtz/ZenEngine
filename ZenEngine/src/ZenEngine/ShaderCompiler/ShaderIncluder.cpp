#include "ShaderIncluder.h"

#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Filesystem.h"

namespace ZenEngine
{
    shaderc_include_result *ShaderIncluder::GetInclude(const char *inRequestedSrc, shaderc_include_type inType, const char *inRequestingSrc, size_t inIncludeDepth)
    {
        const std::string name = std::string(inRequestedSrc);
        ZE_CORE_TRACE("Requested include {}", name);
        if (!std::filesystem::exists("resources/Shaders/" + name))
        {
            ZE_CORE_ERROR("Could not find include file {}", name);
            ZE_ASSERT(false);
        }
        const std::string contents = Filesystem::ReadFileToString("resources/Shaders/" + name);
        
        auto data = new shaderc_include_result;
        auto userData = new std::pair<std::string, std::string>(name, contents);
        data->user_data = userData;

        data->source_name = userData->first.c_str();
        data->source_name_length = userData->first.size();

        data->content = userData->second.c_str();
        data->content_length = userData->second.size();

        return data;
    }
}