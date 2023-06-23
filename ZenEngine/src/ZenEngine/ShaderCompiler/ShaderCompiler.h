#pragma once

#include <shaderc/shaderc.hpp>

namespace ZenEngine
{
    class ShaderCompiler
    {
    public:
        struct ShaderSPIRV
        {
            std::vector<uint32_t> VertexSPIRV;
            std::vector<uint32_t> PixelSPIRV;
        };

        ShaderCompiler(const std::string &inName) : mName(inName) {}

        ShaderSPIRV Compile(const std::string &inSource);
    private:   
        std::string mName;
        shaderc::Compiler mCompiler;
        shaderc::CompileOptions mOptions;
    };
}

