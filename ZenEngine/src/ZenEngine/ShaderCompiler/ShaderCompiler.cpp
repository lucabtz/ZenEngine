#include "ShaderCompiler.h"
#include "ShaderIncluder.h"

#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{

    ShaderCompiler::ShaderSPIRV ShaderCompiler::Compile(const std::string &inSource)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetIncluder(std::make_unique<ShaderIncluder>());
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
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
        ShaderSPIRV result;
        result.VertexSPIRV = std::move(vertexShaderSPIRV);
        result.PixelSPIRV = std::move(pixelShaderSPIRV);
        return result;
    }
}