#include "OpenGLShader.h"

#include <shaderc/shaderc.hpp>
#include <filesystem>
#include <glad/glad.h>
#include "ZenEngine/Core/Filesystem.h"
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{

    static const char *GetCacheDirectory()
    {
        return "Cache/Shaders/OpenGL";
    }

    static void CreateCacheDirectory()
    {
        auto dir = GetCacheDirectory();
        if (!std::filesystem::exists(dir))
        {
            std::filesystem::create_directories(dir);
        }
    }

    OpenGLShader::OpenGLShader(const std::string &inFilepath)
    {
        ZE_CORE_TRACE("Loading shader from file {}", inFilepath);
        auto shaderSource = Filesystem::ReadFileToString(inFilepath);
        std::filesystem::path shaderFilePath = inFilepath;
        mName = shaderFilePath.filename().replace_extension("").string();
        CreateShader(shaderSource);
    }

    OpenGLShader::OpenGLShader(const std::string &inName, const std::string &inSrc)
        : mName(inName)
    {
        CreateShader(inSrc);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(mRendererId);
    }

    void OpenGLShader::Bind() const
    {
        glUseProgram(mRendererId);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string &inName, int inValue)
    {
    }

    void OpenGLShader::SetIntArray(const std::string &inName, int *inValues, uint32_t inCount)
    {
    }

    void OpenGLShader::SetFloat(const std::string &inName, float inValue)
    {
    }

    void OpenGLShader::SetFloat2(const std::string &inName, const glm::vec2 &inValue)
    {
    }

    void OpenGLShader::SetFloat3(const std::string &inName, const glm::vec3 &inValue)
    {
    }

    void OpenGLShader::SetFloat4(const std::string &inName, const glm::vec4 &inValue)
    {
    }

    void OpenGLShader::SetMat4(const std::string &inName, const glm::mat4 &inValue)
    {
    }

    void OpenGLShader::CreateShader(const std::string &inSrc)
    {
        CreateCacheDirectory();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        options.SetSourceLanguage(shaderc_source_language_hlsl);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        
        // first we have to compile the vertex shader
        shaderc::SpvCompilationResult vertexRes = compiler.CompileGlslToSpv(inSrc.c_str(), inSrc.length(), shaderc_vertex_shader, mName.c_str(), "VSMain", options);
        if (vertexRes.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZE_CORE_ERROR("Failed compiling {} shader vertex: {}", mName, vertexRes.GetErrorMessage());
            ZE_ASSERT(false);
        }
        
        // then we compile the pixel shader
        shaderc::SpvCompilationResult pixelRes = compiler.CompileGlslToSpv(inSrc.c_str(), inSrc.length(), shaderc_fragment_shader, mName.c_str(), "PSMain", options);
        if (pixelRes.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZE_CORE_ERROR("Failed compiling {} shader pixel: {}", mName, pixelRes.GetErrorMessage());
            ZE_ASSERT(false);
        }

        std::vector<uint32_t> vertexShaderSPIRV(vertexRes.cbegin(), vertexRes.cend());
        std::vector<uint32_t> pixelShaderSPIRV(pixelRes.cbegin(), pixelRes.cend());
        

        // TODO: at the moment we just write files to the cache but the cache is not really used
        // this is useful for SPIR-V visualization and debug but in the future of course the cache should be used
        std::filesystem::path cacheDir = GetCacheDirectory();
        std::filesystem::path vsCacheFile = cacheDir / (mName + "Vertex.spirv");
        std::filesystem::path psCacheFile = cacheDir / (mName + "Pixel.spirv");

        Filesystem::WriteBytes(vsCacheFile.string(), (const uint8_t *)vertexShaderSPIRV.data(), vertexShaderSPIRV.size() * sizeof(uint32_t));
        Filesystem::WriteBytes(psCacheFile.string(), (const uint8_t *)pixelShaderSPIRV.data(), pixelShaderSPIRV.size() * sizeof(uint32_t));

        GLuint program = glCreateProgram();

        GLuint ids[2];
        ids[0] = glCreateShader(GL_VERTEX_SHADER);
        glShaderBinary(1, &ids[0], GL_SHADER_BINARY_FORMAT_SPIR_V, vertexShaderSPIRV.data(), vertexShaderSPIRV.size() * sizeof(uint32_t));
        glSpecializeShader(ids[0], "VSMain", 0, nullptr, nullptr);
        glAttachShader(program, ids[0]);

        ids[1] = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderBinary(1, &ids[1], GL_SHADER_BINARY_FORMAT_SPIR_V, pixelShaderSPIRV.data(), pixelShaderSPIRV.size() * sizeof(uint32_t));
        glSpecializeShader(ids[1], "PSMain", 0, nullptr, nullptr);
        glAttachShader(program, ids[1]);

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			ZE_CORE_ERROR("Failed compiling {} shader: {}", mName, infoLog.data());

			glDeleteProgram(program);

			glDeleteShader(ids[0]);
			glDeleteShader(ids[1]);
		}
		
        glDetachShader(program, ids[0]);
		glDeleteShader(ids[0]);
		
        glDetachShader(program, ids[1]);
		glDeleteShader(ids[1]);

		mRendererId = program;
    }
}