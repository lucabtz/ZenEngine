#include "Material.h"

namespace ZenEngine
{
    std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader> &inShaderProgram)
    {
        return std::make_shared<Material>(inShaderProgram);
    }

    void Material::Bind()
    {
        for (auto &[name, param] : mParameters)
        {
            switch (param.Info.Type)
            {
            case MaterialDataType::Float: mShaderProgram->SetFloat(param.Info.Name, std::get<float>(param.Value)); break;
            case MaterialDataType::Float2: mShaderProgram->SetFloat2(param.Info.Name, std::get<glm::vec2>(param.Value)); break;
            case MaterialDataType::Float3: mShaderProgram->SetFloat3(param.Info.Name, std::get<glm::vec3>(param.Value)); break;
            case MaterialDataType::Float4: mShaderProgram->SetFloat4(param.Info.Name, std::get<glm::vec4>(param.Value)); break;
            case MaterialDataType::Int: mShaderProgram->SetInt(param.Info.Name, std::get<int32_t>(param.Value)); break;
            case MaterialDataType::Mat4: mShaderProgram->SetMat4(param.Info.Name, std::get<glm::mat4>(param.Value)); break;
            default: ZE_ASSERT_CORE_MSG(false, "The parameter type is not currently supported!");
            }
        }
        mShaderProgram->Bind();
    }

    void Material::Unbind()
    {
        mShaderProgram->Unbind();
    }

    Material::Material(const std::shared_ptr<Shader> &inShaderProgram)
        : mShaderProgram(inShaderProgram)
    {
        for (auto &[name, info] : inShaderProgram->GetShaderUniformInfo())
        {
            mParameters[name] = { info };
            switch (info.Type)
            {
            case MaterialDataType::Float: mParameters[name].Value = 0.0f; break;
            case MaterialDataType::Float2: mParameters[name].Value = glm::vec2(0.0f); break;
            case MaterialDataType::Float3: mParameters[name].Value = glm::vec3(0.0f); break;
            case MaterialDataType::Float4: mParameters[name].Value = glm::vec4(0.0f); break;
            case MaterialDataType::Int: mParameters[name].Value = (int32_t)0; break;
            case MaterialDataType::Int2: mParameters[name].Value = glm::ivec2(0); break;
            case MaterialDataType::Int3: mParameters[name].Value = glm::ivec3(0); break;
            case MaterialDataType::Int4: mParameters[name].Value = glm::ivec4(0); break;
            case MaterialDataType::Mat3: mParameters[name].Value = glm::mat3(1.0f); break;
            case MaterialDataType::Mat4: mParameters[name].Value = glm::mat4(1.0f); break;
            case MaterialDataType::Bool: mParameters[name].Value = false; break;
            default: ZE_CORE_WARN("Material.cpp: This parameter is not a variant type!");
            }
        }
    }

}