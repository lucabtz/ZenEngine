#pragma once

#include <variant>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"
#include "ZenEngine/ShaderCompiler/ShaderReflector.h"
#include "ZenEngine/Core/Macros.h" 

namespace ZenEngine
{

    using MaterialParameterInfo = ShaderReflector::VariableInfo;
    using MaterialDataType = ShaderReflector::ShaderType;
    using MaterialTextureInfo = ShaderReflector::TextureInfo;

    struct MaterialParameter
    {
        using ValueType = std::variant<
            float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, 
            int32_t, glm::ivec2, glm::ivec3, glm::ivec4, 
            bool
        >;
        MaterialParameterInfo Info;
        ValueType Value;
    };

    struct MaterialTexture
    {
        MaterialTextureInfo Info;
        std::shared_ptr<Texture2D> Texture;
    };

#pragma region "Type conversions"
    template <MaterialDataType DataType>
    struct MaterialDataTypeCppType
    {};

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Float>
    {
        using Type = float;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Float2>
    {
        using Type = glm::vec2;
    };


    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Float3>
    {
        using Type = glm::vec3;
    };

    
    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Float4>
    {
        using Type = glm::vec4;
    };

    
    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Int>
    {
        using Type = int32_t;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Int2>
    {
        using Type = glm::ivec2;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Int3>
    {
        using Type = glm::ivec3;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Int4>
    {
        using Type = glm::ivec4;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Bool>
    {
        using Type = bool;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Mat3>
    {
        using Type = glm::mat3;
    };

    template <>
    struct MaterialDataTypeCppType<MaterialDataType::Mat4>
    {
        using Type = glm::mat4;
    };
#pragma endregion

    class Material
    {
    public:
        Material(const std::shared_ptr<Shader> &inShaderProgram);
        static std::shared_ptr<Material> Create(const std::shared_ptr<Shader> &inShaderProgram);

        template <MaterialDataType DataType>
        void Set(const std::string &inName, const typename MaterialDataTypeCppType<DataType>::Type &inValue)
        {
            ZE_ASSERT_CORE_MSG(mParameters.contains(inName), "Parameter does not exist!");
            mParameters[inName].Value = inValue;
        }

        template <MaterialDataType DataType>
        auto Get(const std::string &inName)
        {
            ZE_ASSERT_CORE_MSG(mParameters.contains(inName), "Parameter does not exist!");
            return std::get<typename MaterialDataTypeCppType<DataType>::Type>(mParameters[inName].Value);
        }

        void SetTexture(const std::string &inName, const std::shared_ptr<Texture2D> &inTexture);

        const std::unordered_map<std::string, MaterialParameter> &GetParameters() const { return mParameters; }
        const std::unordered_map<std::string, MaterialTexture> &GetTextures() const { return mTextures; } 

        void Bind();
        void Unbind();

    private:
        std::unordered_map<std::string, MaterialParameter> mParameters;
        std::unordered_map<std::string, MaterialTexture> mTextures;
    
        std::shared_ptr<Shader> mShaderProgram;
    };
}