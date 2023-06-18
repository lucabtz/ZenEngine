#include "ShaderIncluder.h"

#include "ZenEngine/Core/Filesystem.h"

namespace ZenEngine
{
    shaderc_include_result *ShaderIncluder::GetInclude(const char *inRequestedSrc, shaderc_include_type inType, const char *inRequestingSrc, size_t inIncludeDepth)
    {
        const std::string name = std::string(inRequestedSrc);
        const std::string contents = Filesystem::ReadFileToString("resources/Shaders/" + name);

        auto data = new shaderc_include_result;

        data->source_name = name.c_str();
        data->source_name_length = name.size();

        data->content = contents.c_str();
        data->content_length = contents.size();

        return data;
    }
}