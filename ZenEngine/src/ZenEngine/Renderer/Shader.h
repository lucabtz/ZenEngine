#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "Renderer.h"

namespace ZenEngine
{
    class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string &inName, int inValue) = 0;
		virtual void SetIntArray(const std::string &inName, int* inValues, uint32_t inCount) = 0;
		virtual void SetFloat(const std::string &inName, float inValue) = 0;
		virtual void SetFloat2(const std::string &inName, const glm::vec2 &inValue) = 0;
		virtual void SetFloat3(const std::string &inName, const glm::vec3 &inValue) = 0;
		virtual void SetFloat4(const std::string &inName, const glm::vec4 &inValue) = 0;
		virtual void SetMat4(const std::string &inName, const glm::mat4 &inValue) = 0;

		static std::shared_ptr<Shader> Create(const std::string &inFilepath);
		static std::shared_ptr<Shader> Create(const std::string &inName, const std::string &inSrc);
	};

	PROXY(Shader)
	{
		SETUP_PROXY(Shader)
	public:
		CREATE_METHOD_COMMAND_ONE_PARAM(Shader_Create1, const std::string, Filepath)
		static Proxy Create(const std::string &inFilepath)
		{
			Proxy p(new Resource);
			Renderer::Get().Submit<Shader_Create1>(p.mHandle, inFilepath);
			return p;
		}

		CREATE_METHOD_COMMAND_TWO_PARAM(Shader_Create2, const std::string, Name, const std::string, Src)
		static Proxy Create(const std::string &inName, const std::string &inSrc)
		{
			Proxy p(new Resource);
			Renderer::Get().Submit<Shader_Create2>(p.mHandle, inName, inSrc);
			return p;
		}

		CREATE_METHOD_COMMAND_NO_PARAM(Shader_Bind)
		void Bind()
		{
			Renderer::Get().Submit<Shader_Bind>(mHandle);
		}

		CREATE_METHOD_COMMAND_NO_PARAM(Shader_Unbind)
		void Unbind()
		{
			Renderer::Get().Submit<Shader_Unbind>(mHandle);
		}

		//TODO: other methods
	};

}