#pragma once

#include <shaderc/shaderc.hpp>

namespace ZenEngine
{

    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
        shaderc_include_result* GetInclude(const char* inRequestedSrc, shaderc_include_type inType, const char* inRequestingSrc, size_t inIncludeDepth) override;

        void ReleaseInclude(shaderc_include_result* inData) override
        {
            delete inData->user_data;
            delete inData;
        };
    };
}