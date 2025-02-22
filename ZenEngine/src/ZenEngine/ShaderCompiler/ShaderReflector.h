#pragma once

#include <string>
#include <vector>
#include <spirv_cross/spirv_hlsl.hpp>

namespace ZenEngine
{

    class ShaderReflector
    {
    public:
    	enum class ShaderType
        {
            None = 0, Void, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Struct, Custom
        };

        struct VariableInfo
        {
            std::string Name;
            ShaderType Type;
            uint32_t Size;
            uint32_t Offset;
        
            spirv_cross::SPIRType Type_Internal;
            spirv_cross::TypeID TypeId_Internal;

            // hack to make it work with newer spirv_cross version where spirv_cross::SPIRType does not have a default constructor
            VariableInfo(): Type_Internal(spv::Op::OpNop) {}
        };
        
        struct UniformBufferInfo
        {
            std::string Name;
            uint32_t Size;
            uint32_t Binding;
            std::vector<VariableInfo> Members;
        };

        struct TextureInfo
        {
            std::string Name;
            uint32_t Binding;
        };

        struct ReflectionResult
        {
            std::vector<UniformBufferInfo> UniformBuffers;
            std::vector<TextureInfo> Textures;
        };

        ShaderReflector(std::vector<uint32_t> inSpirvSrc) : mCompiler(std::move(inSpirvSrc)) {}

        ReflectionResult Reflect();

        std::vector<VariableInfo> ReflectStruct(const VariableInfo &inInfo);

#define ADD_TYPE_STRING_CONVERSION(type) case ShaderType::type: return #type
        static std::string ShaderTypeToString(ShaderType inType)
        {
            switch (inType)
            {
            ADD_TYPE_STRING_CONVERSION(None);
            ADD_TYPE_STRING_CONVERSION(Void);
            ADD_TYPE_STRING_CONVERSION(Float);
            ADD_TYPE_STRING_CONVERSION(Float2);
            ADD_TYPE_STRING_CONVERSION(Float3);
            ADD_TYPE_STRING_CONVERSION(Float4);
            ADD_TYPE_STRING_CONVERSION(Int);
            ADD_TYPE_STRING_CONVERSION(Int2);
            ADD_TYPE_STRING_CONVERSION(Int3);
            ADD_TYPE_STRING_CONVERSION(Int4);
            ADD_TYPE_STRING_CONVERSION(Mat3);
            ADD_TYPE_STRING_CONVERSION(Mat4);
            ADD_TYPE_STRING_CONVERSION(Bool);
            ADD_TYPE_STRING_CONVERSION(Struct);
            ADD_TYPE_STRING_CONVERSION(Custom);
            }
        }
#undef ADD_TYPE_STRING_CONVERSION
    
    private:
        spirv_cross::CompilerHLSL mCompiler;
        
        std::vector<VariableInfo> ReflectStruct_Internal(const spirv_cross::SPIRType &inType, const spirv_cross::TypeID &inTypeId);
    };
}