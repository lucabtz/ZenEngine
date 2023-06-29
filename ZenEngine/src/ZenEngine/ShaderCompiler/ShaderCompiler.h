#pragma once

#include <filesystem>
#include <shaderc/shaderc.hpp>
#include "ZenEngine/Core/Macros.h"
#include "ShaderReflector.h"

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

        struct GLSLShaderSource
        {
            std::string VertexSource;
            std::string PixelSource;
        };

        struct CompilationResult
        {
            ShaderReflector::ReflectionResult VertexReflectionInfo;
            ShaderReflector::ReflectionResult PixelReflectionInfo;
            ShaderSPIRV SPIRV;
        };

        enum class ShaderStage { Vertex, Pixel };

        ShaderCompiler(const std::string &inName) : mName(inName) {}

        ShaderSPIRV CompileHLSLToVulkan(const std::string &inSource);
        GLSLShaderSource CompileVulkanSPIRVToGLSL(ShaderSPIRV inVulkanSPIRV);
        ShaderSPIRV CompileGLSLToOpenGLSPIRV(const GLSLShaderSource &inGLSLSource);

        CompilationResult Compile(const std::string &inSource);
    private:   
        std::string mName;

        static const std::filesystem::path sCacheFolder;
    
        void CacheVulkanBinary(const ShaderSPIRV &inSPIRV) { CacheBinary(inSPIRV, sCacheFolder / "Vulkan"); }
        void CacheOpenGLBinary(const ShaderSPIRV &inSPIRV) { CacheBinary(inSPIRV, sCacheFolder / "OpenGL"); }

        void CacheBinary(const ShaderSPIRV &inSPIRV, const std::filesystem::path &inDestinationFolder);
        
        void CreateFolderStructure();
    };
}

