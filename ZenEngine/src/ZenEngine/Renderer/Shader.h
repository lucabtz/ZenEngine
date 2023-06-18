#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace ZenEngine
{
    class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string &inName, int inValue) = 0;
		virtual void SetFloat(const std::string &inName, float inValue) = 0;
		virtual void SetFloat2(const std::string &inName, const glm::vec2 &inValue) = 0;
		virtual void SetFloat3(const std::string &inName, const glm::vec3 &inValue) = 0;
		virtual void SetFloat4(const std::string &inName, const glm::vec4 &inValue) = 0;
		virtual void SetMat4(const std::string &inName, const glm::mat4 &inValue) = 0;

		static std::shared_ptr<Shader> Create(const std::string &inFilepath);
		static std::shared_ptr<Shader> Create(const std::string &inName, const std::string &inSrc);
	};

}