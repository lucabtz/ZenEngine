#pragma once

#include "ZenEngine/Renderer/Shader.h"
#include "ZenEngine/Renderer/UniformBuffer.h"
#include "ZenEngine/ShaderCompiler/ShaderReflector.h"
#include <string>
#include <vector>

namespace ZenEngine 
{

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string &inFilepath);
		OpenGLShader(const std::string &inName, const std::string &inSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string &inName, int inValue) override;
		virtual void SetFloat(const std::string &inName, float inValue) override;
		virtual void SetFloat2(const std::string &inName, const glm::vec2 &inValue) override;
		virtual void SetFloat3(const std::string &inName, const glm::vec3 &inValue) override;
		virtual void SetFloat4(const std::string &inName, const glm::vec4 &inValue) override;
		virtual void SetMat4(const std::string &inName, const glm::mat4 &inValue) override;
	private:
		std::string mName;
		uint32_t mRendererId;
		std::unordered_map<std::string, ShaderReflector::VariableInfo> mUniforms;

		std::shared_ptr<UniformBuffer> mUniformBuffer;

		void CreateShader(const std::string &inSrc);
		void PrintReflectInfo(std::vector<uint32_t> inSpirvSrc, const std::string &inStageName);
		void Reflect(std::vector<uint32_t> inSpirvSrc);

		void SetUniform(const std::string &inName, void *inData, ShaderReflector::ShaderType inShaderType);

    };

}