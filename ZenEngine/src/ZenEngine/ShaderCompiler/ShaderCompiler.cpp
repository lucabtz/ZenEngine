#include "ShaderCompiler.h"
#include "ShaderIncluder.h"

#include <spirv_cross/spirv_glsl.hpp>
#include "ZenEngine/Core/Filesystem.h"

namespace ZenEngine
{
    const std::filesystem::path ShaderCompiler::sCacheFolder = "Cache";

    ShaderCompiler::ShaderSPIRV ShaderCompiler::CompileHLSLToVulkan(const std::string &inSource)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetIncluder(std::make_unique<ShaderIncluder>());
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
        options.SetSourceLanguage(shaderc_source_language_hlsl);
        
        // first we have to compile the vertex shader
        auto preProcessedVertex = compiler.PreprocessGlsl(inSource, shaderc_vertex_shader, mName.c_str(), options);
        std::string preProcessedVtxSrc(preProcessedVertex.begin(), preProcessedVertex.end());
        shaderc::SpvCompilationResult vertexRes = compiler.CompileGlslToSpv(preProcessedVtxSrc.c_str(), preProcessedVtxSrc.length(), shaderc_vertex_shader, mName.c_str(), "VSMain", options);
        if (vertexRes.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZE_CORE_ERROR("Failed compiling {} shader vertex: {}", mName, vertexRes.GetErrorMessage());
            ZE_ASSERT(false);
        }
        
        // then we compile the pixel shader
        auto preProcessedPixel = compiler.PreprocessGlsl(inSource, shaderc_fragment_shader, mName.c_str(), options);
        std::string preProcessedPixSrc(preProcessedPixel.begin(), preProcessedPixel.end());
        shaderc::SpvCompilationResult pixelRes = compiler.CompileGlslToSpv(preProcessedPixSrc.c_str(), preProcessedPixSrc.length(), shaderc_fragment_shader, mName.c_str(), "PSMain", options);
        if (pixelRes.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZE_CORE_ERROR("Failed compiling {} shader pixel: {}", mName, pixelRes.GetErrorMessage());
            ZE_ASSERT(false);
        }

        std::vector<uint32_t> vertexShaderSPIRV(vertexRes.cbegin(), vertexRes.cend());
        std::vector<uint32_t> pixelShaderSPIRV(pixelRes.cbegin(), pixelRes.cend());
        ShaderSPIRV vulkanSPIRV;
        vulkanSPIRV.VertexSPIRV = std::move(vertexShaderSPIRV);
        vulkanSPIRV.PixelSPIRV = std::move(pixelShaderSPIRV);
        
        return vulkanSPIRV;
    }

    static void InheritCombinedSamplerBindings(spirv_cross::CompilerGLSL &inCompiler)
    {
        // from spirv_cross_util.cpp

        auto &samplers = inCompiler.get_combined_image_samplers();
        for (auto &s : samplers)
        {
            if (inCompiler.has_decoration(s.image_id, spv::DecorationDescriptorSet))
            {
                uint32_t set = inCompiler.get_decoration(s.image_id, spv::DecorationDescriptorSet);
                inCompiler.set_decoration(s.combined_id, spv::DecorationDescriptorSet, set);
            }

            if (inCompiler.has_decoration(s.image_id, spv::DecorationBinding))
            {
                uint32_t binding = inCompiler.get_decoration(s.image_id, spv::DecorationBinding);
                inCompiler.set_decoration(s.combined_id, spv::DecorationBinding, binding);
            }
        }
    }

    static std::string CompileVulkanSPIRVToGLSL_Internal(std::vector<uint32_t> inVulkanSPIRV)
    {
        spirv_cross::CompilerGLSL glslCompiler(std::move(inVulkanSPIRV));
        try
        {
            glslCompiler.build_combined_image_samplers();
            InheritCombinedSamplerBindings(glslCompiler);
            return glslCompiler.compile();
        }
        catch(const spirv_cross::CompilerError& e)
        {
            ZE_CORE_ERROR("SPIRV-Cross Compiler Error {}", e.what());
            ZE_ASSERT(false);
        }        
    }

    ShaderCompiler::GLSLShaderSource ShaderCompiler::CompileVulkanSPIRVToGLSL(ShaderSPIRV inVulkanSPIRV)
    {
        GLSLShaderSource source;
        source.VertexSource = CompileVulkanSPIRVToGLSL_Internal(std::move(inVulkanSPIRV.VertexSPIRV));
        source.PixelSource = CompileVulkanSPIRVToGLSL_Internal(std::move(inVulkanSPIRV.PixelSPIRV));
        return source;
    }
    
    static shaderc_shader_kind ShaderStageToShadercKind(ShaderCompiler::ShaderStage inStage)
    {
        switch (inStage)
        {
        case ShaderCompiler::ShaderStage::Vertex: return shaderc_vertex_shader;
        case ShaderCompiler::ShaderStage::Pixel: return shaderc_fragment_shader;
        }
    }

    static std::vector<uint32_t> CompileGLSLToOpenGLSPIRV_Internal(const std::string inSource, ShaderCompiler::ShaderStage inStage, const std::string &inName)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        options.SetSourceLanguage(shaderc_source_language_glsl);

        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(inSource, ShaderStageToShadercKind(inStage), inName.c_str());
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZE_CORE_ERROR("Error: {}", result.GetErrorMessage());
            ZE_ASSERT(false);
        }
        std::vector<uint32_t> spirv(result.begin(), result.end());
        return spirv;
    }

    ShaderCompiler::ShaderSPIRV ShaderCompiler::CompileGLSLToOpenGLSPIRV(const GLSLShaderSource &inGLSLSource)
    {
        ShaderSPIRV result;
        result.VertexSPIRV = CompileGLSLToOpenGLSPIRV_Internal(inGLSLSource.VertexSource, ShaderStage::Vertex, mName);
        result.PixelSPIRV = CompileGLSLToOpenGLSPIRV_Internal(inGLSLSource.PixelSource, ShaderStage::Pixel, mName);
        return result;
    }

    ShaderCompiler::CompilationResult ShaderCompiler::Compile(const std::string &inSource)
    {
        CreateFolderStructure();
        CompilationResult result;

        // TODO check cache
        auto vulkanSPIRV = CompileHLSLToVulkan(inSource);
        ShaderReflector vertReflector(vulkanSPIRV.VertexSPIRV);
        ShaderReflector pixlReflector(vulkanSPIRV.PixelSPIRV);
        result.VertexReflectionInfo = vertReflector.Reflect();
        result.PixelReflectionInfo = pixlReflector.Reflect();
        CacheVulkanBinary(vulkanSPIRV);

        auto glslSource = CompileVulkanSPIRVToGLSL(std::move(vulkanSPIRV));

        ZE_CORE_TRACE("Shader {} GLSL source\n\n{}\n\n{}", mName, glslSource.VertexSource, glslSource.PixelSource);

        // TODO check cache
        result.SPIRV = CompileGLSLToOpenGLSPIRV(glslSource);
        CacheOpenGLBinary(result.SPIRV);
        return result;
    }

    void ShaderCompiler::CacheBinary(const ShaderSPIRV &inSPIRV, const std::filesystem::path &inDestinationFolder)
    {
        if (!std::filesystem::exists(inDestinationFolder))
            std::filesystem::create_directories(inDestinationFolder);
        Filesystem::WriteBytes(inDestinationFolder / (mName + "Vertex.spirv"), reinterpret_cast<const uint8_t*>(inSPIRV.VertexSPIRV.data()), inSPIRV.VertexSPIRV.size() * sizeof(uint32_t));
        Filesystem::WriteBytes(inDestinationFolder / (mName + "Pixel.spirv"), reinterpret_cast<const uint8_t*>(inSPIRV.PixelSPIRV.data()), inSPIRV.PixelSPIRV.size() * sizeof(uint32_t));
    }

    void ShaderCompiler::CreateFolderStructure()
    {
        if (!std::filesystem::exists(sCacheFolder))
            std::filesystem::create_directories(sCacheFolder);
    }
}