#include "ShaderReflector.h"

#include "ZenEngine/Core/Log.h"

namespace ZenEngine
{

    static ShaderReflector::ShaderType SPIRVReflectTypeToShaderType(spirv_cross::SPIRType::BaseType inBaseType, uint32_t inVecsize, uint32_t inColumns)
    {
        switch (inBaseType)
        {
		case spirv_cross::SPIRType::BaseType::Void: return ShaderReflector::ShaderType::Void;
		case spirv_cross::SPIRType::BaseType::Boolean: return ShaderReflector::ShaderType::Bool;
		case spirv_cross::SPIRType::BaseType::Int:
        {
            if (inColumns > 1) return ShaderReflector::ShaderType::Custom; // no int matrices
            switch (inVecsize)
            {
            case 1: return ShaderReflector::ShaderType::Int;
            case 2: return ShaderReflector::ShaderType::Int2;
            case 3: return ShaderReflector::ShaderType::Int3;
            case 4: return ShaderReflector::ShaderType::Int4;
            }
            return ShaderReflector::ShaderType::Custom;
        }

        case spirv_cross::SPIRType::BaseType::Float:
		{
            switch (inVecsize)
            {
            case 1: return ShaderReflector::ShaderType::Float;
            case 2: return ShaderReflector::ShaderType::Float2;
            case 3:
            {
                if (inColumns == 3) return ShaderReflector::ShaderType::Mat3;
                else if (inColumns == 1) return ShaderReflector::ShaderType::Float3;
            }
            case 4: 
            {
                if (inColumns == 4) return ShaderReflector::ShaderType::Mat4;
                else if (inColumns == 1) return ShaderReflector::ShaderType::Float4;
            }
            }
            return ShaderReflector::ShaderType::Custom;
        }

        case spirv_cross::SPIRType::BaseType::Struct: return ShaderReflector::ShaderType::Struct;
		
        case spirv_cross::SPIRType::BaseType::Image:
		case spirv_cross::SPIRType::BaseType::SampledImage:
		case spirv_cross::SPIRType::BaseType::Sampler:
		
		// currently unsupported types
        case spirv_cross::SPIRType::BaseType::Char:
		case spirv_cross::SPIRType::BaseType::Double:
		case spirv_cross::SPIRType::BaseType::UInt:
		case spirv_cross::SPIRType::BaseType::Int64:
		case spirv_cross::SPIRType::BaseType::UInt64:
		case spirv_cross::SPIRType::BaseType::Half:
        case spirv_cross::SPIRType::BaseType::SByte:
		case spirv_cross::SPIRType::BaseType::UByte:
		case spirv_cross::SPIRType::BaseType::Short:
		case spirv_cross::SPIRType::BaseType::UShort:
		
        case spirv_cross::SPIRType::BaseType::Interpolant:
		case spirv_cross::SPIRType::BaseType::ControlPointArray:
		case spirv_cross::SPIRType::BaseType::RayQuery:
		case spirv_cross::SPIRType::BaseType::AccelerationStructure:
		case spirv_cross::SPIRType::BaseType::AtomicCounter:
        case spirv_cross::SPIRType::BaseType::Unknown: return ShaderReflector::ShaderType::Custom;
        }
        return ShaderReflector::ShaderType::Custom;
    }

    ShaderReflector::ReflectionResult ShaderReflector::Reflect()
    {
        spirv_cross::ShaderResources resources = mCompiler.get_shader_resources();
        ReflectionResult result{};

        for (auto &uniformBuffer : resources.uniform_buffers)
        {
            UniformBufferInfo ubInfo;
            ubInfo.Name = uniformBuffer.name;
            ubInfo.Size = mCompiler.get_declared_struct_size(mCompiler.get_type(uniformBuffer.base_type_id));
            ubInfo.Binding = mCompiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);
            ubInfo.Members = ReflectStruct_Internal(mCompiler.get_type(uniformBuffer.base_type_id), uniformBuffer.base_type_id);
            result.UniformBuffers.push_back(std::move(ubInfo));
        }

        for (auto &img : resources.sampled_images)
        {
            TextureInfo ti;
            ti.Name = img.name;
            ti.Binding = mCompiler.get_decoration(img.id, spv::DecorationBinding);
            result.Textures.push_back(ti);
        }

        for (auto& img : resources.separate_images)
        {
            TextureInfo ti;
            ti.Name = img.name;
            ti.Binding = mCompiler.get_decoration(img.id, spv::DecorationBinding);
            result.Textures.push_back(ti);
        }

        return result;
    }

    std::vector<ShaderReflector::VariableInfo> ShaderReflector::ReflectStruct_Internal(const spirv_cross::SPIRType &inType, const spirv_cross::TypeID &inTypeId)
    {
        auto size = mCompiler.get_declared_struct_size(inType);
        std::vector<VariableInfo> result;

        uint32_t currentSize = 0;
        uint32_t index = 0;
        while (currentSize < size)
        {
            VariableInfo vi{};
            vi.Name = mCompiler.get_member_name(inTypeId, index);
            vi.Size = mCompiler.get_declared_struct_member_size(inType, index);
            vi.TypeId_Internal = mCompiler.get_type(inTypeId).member_types[index];
            vi.Type_Internal = mCompiler.get_type(vi.TypeId_Internal);
            vi.Type = SPIRVReflectTypeToShaderType(vi.Type_Internal.basetype, vi.Type_Internal.vecsize, vi.Type_Internal.columns);
            vi.Offset = currentSize;
            currentSize += vi.Size;
            result.push_back(std::move(vi));
            index += 1;
        }

        return result;
    }

    std::vector<ShaderReflector::VariableInfo> ShaderReflector::ReflectStruct(const VariableInfo &inInfo)
    {
        ZE_ASSERT_CORE_MSG(inInfo.Type == ShaderType::Struct, "The given variable is not a struct!");
        return ReflectStruct_Internal(inInfo.Type_Internal, inInfo.TypeId_Internal);
    }
}
