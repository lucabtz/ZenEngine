#include "OpenGLShader.h"

#include <shaderc/shaderc.hpp>
#include <filesystem>
#include <glad/glad.h>
#include <spirv_cross/spirv_hlsl.hpp>

#include "ZenEngine/Core/Filesystem.h"
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/ShaderCompiler/ShaderCompiler.h"

namespace ZenEngine
{

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
        if (mUniformBuffer != nullptr) mUniformBuffer->Bind();
        glUseProgram(mRendererId);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::SetUniform(const std::string &inName, void *inData, ShaderReflector::ShaderType inShaderType)
    {
        if (mUniforms.contains(inName))
        {
            auto &uniformData = mUniforms[inName];
            ZE_ASSERT_CORE_MSG(uniformData.Type == inShaderType, "{} is not at int", inName);
            mUniformBuffer->SetData(inData, uniformData.Size, uniformData.Offset);
        }
    }

    void OpenGLShader::SetInt(const std::string &inName, int inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Int);
    }

    void OpenGLShader::SetFloat(const std::string &inName, float inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Float);
    }

    void OpenGLShader::SetFloat2(const std::string &inName, const glm::vec2 &inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Float2);
    }

    void OpenGLShader::SetFloat3(const std::string &inName, const glm::vec3 &inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Float3);
    }

    void OpenGLShader::SetFloat4(const std::string &inName, const glm::vec4 &inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Float4);
    }

    void OpenGLShader::SetMat4(const std::string &inName, const glm::mat4 &inValue)
    {
        SetUniform(inName, (void*)&inValue, ShaderReflector::ShaderType::Mat4);
    }

    void OpenGLShader::CreateShader(const std::string &inSrc)
    {
        ShaderCompiler compiler(mName);
        auto res = compiler.Compile(inSrc);
        Reflect(res.VertexReflectionInfo);
        Reflect(res.PixelReflectionInfo);
        CreateShader(res.SPIRV.VertexSPIRV, res.SPIRV.PixelSPIRV); 
    }

    void OpenGLShader::CreateShader(const std::vector<uint32_t> &inVtxSPIRV, const std::vector<uint32_t> &inPixSPIRV)
    {
        GLuint program = glCreateProgram();

        GLuint ids[2];
        ids[0] = glCreateShader(GL_VERTEX_SHADER);
        glShaderBinary(1, &ids[0], GL_SHADER_BINARY_FORMAT_SPIR_V, inVtxSPIRV.data(), inVtxSPIRV.size() * sizeof(uint32_t));
        glSpecializeShader(ids[0], "main", 0, nullptr, nullptr);
        glAttachShader(program, ids[0]);

        ids[1] = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderBinary(1, &ids[1], GL_SHADER_BINARY_FORMAT_SPIR_V, inPixSPIRV.data(), inPixSPIRV.size() * sizeof(uint32_t));
        glSpecializeShader(ids[1], "main", 0, nullptr, nullptr);
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

    void OpenGLShader::Reflect(const ShaderReflector::ReflectionResult &inResult)
    {
        auto it = std::find_if(inResult.UniformBuffers.begin(), inResult.UniformBuffers.end(), [](auto &ubInfo){ return ubInfo.Name == "$Global"; });
        if (it != inResult.UniformBuffers.end())
        {
            if (mUniformBuffer == nullptr) mUniformBuffer = UniformBuffer::Create(it->Size, it->Binding);
            for (auto &uniform : it->Members)
            {
                if (mUniforms.contains(uniform.Name)) continue;
                ZE_CORE_TRACE("Adding uniform {}", uniform.Name);
                mUniforms[uniform.Name] = uniform;
            }
        }

        for (auto &texInfo : inResult.Textures)
            mTextures[texInfo.Name] = texInfo;

#pragma region "Printing"
        ZE_CORE_INFO("Uniform buffers");
        for (auto &ub : inResult.UniformBuffers)
        {
            ZE_CORE_INFO("\t{}, Size {}, Binding {}", ub.Name, ub.Size, ub.Binding);
            for (auto &member : ub.Members)
            {
                ZE_CORE_INFO("\t\t- {} {}, Size {}", ShaderReflector::ShaderTypeToString(member.Type), member.Name, member.Size);
            }
        }
        for (auto &texture : inResult.Textures)
        {
            ZE_CORE_INFO("\t{} Binding {}", texture.Name, texture.Binding);
        }
#pragma endregion
    }
}