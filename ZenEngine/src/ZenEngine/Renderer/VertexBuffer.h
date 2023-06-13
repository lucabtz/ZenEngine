#pragma once

#include "ZenEngine/Core/Macros.h"
#include "RenderCommand.h"

namespace ZenEngine
{
    
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
		}

		ZE_ASSERT_CORE_MSG(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType inType, const std::string& inName, bool inNormalized = false)
			: Name(inName), Type(inType), Size(ShaderDataTypeSize(inType)), Offset(0), Normalized(inNormalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Float2:  return 2;
            case ShaderDataType::Float3:  return 3;
            case ShaderDataType::Float4:  return 4;
            case ShaderDataType::Mat3:    return 3; // 3* float3
            case ShaderDataType::Mat4:    return 4; // 4* float4
            case ShaderDataType::Int:     return 1;
            case ShaderDataType::Int2:    return 2;
            case ShaderDataType::Int3:    return 3;
            case ShaderDataType::Int4:    return 4;
            case ShaderDataType::Bool:    return 1;
			}

			ZE_ASSERT_CORE_MSG(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<BufferElement> elements)
			: mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return mStride; }
		const std::vector<BufferElement>& GetElements() const { return mElements; }

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.Offset = offset;
				offset += element.Size;
				mStride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;
	};

    class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* inData, uint32_t inSize) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& inLayout) = 0;

		static std::shared_ptr<VertexBuffer> Create(uint32_t inSize);
		static std::shared_ptr<VertexBuffer> Create(const float *inVertices, uint32_t inSize);
	};

	PROXY(VertexBuffer) 
	{
		SETUP_PROXY(VertexBuffer)
	public:
		CREATE_METHOD_COMMAND_ONE_PARAM(VertexBuffer_Create1, uint32_t, Size)
		static Proxy Create(uint32_t inSize) 
		{ 
			Proxy p(new Resource);
			Renderer::Get().Submit<VertexBuffer_Create1>(p.mHandle, inSize);
			return p;
		}

		CREATE_METHOD_COMMAND_TWO_PARAM(VertexBuffer_Create2, const std::vector<float>, Vertices, uint32_t, Size)
		static Proxy Create(const std::vector<float> &inVertices, uint32_t inSize)
		{
			Proxy p(new Resource);
			Renderer::Get().Submit<VertexBuffer_Create2>(p.mHandle, inVertices, inSize);
			return p;
		}

		CREATE_METHOD_COMMAND_NO_PARAM(VertexBuffer_Bind)
		void Bind() const
		{
			Renderer::Get().Submit<VertexBuffer_Bind>(mHandle);
		}

		CREATE_METHOD_COMMAND_NO_PARAM(VertexBuffer_Unbind)
		void Unbind() const
		{
			Renderer::Get().Submit<VertexBuffer_Unbind>(mHandle);
		}

		CREATE_METHOD_COMMAND_TWO_PARAM(VertexBuffer_SetData, const void *, Data, uint32_t, Size)
		void SetData(const void* inData, uint32_t inSize)
		{
			Renderer::Get().Submit<VertexBuffer_SetData>(mHandle, inData, inSize);
		}


		CREATE_METHOD_COMMAND_ONE_PARAM(VertexBuffer_SetLayout, const BufferLayout, Layout)
		void SetLayout(const BufferLayout& inLayout)
		{
			Renderer::Get().Submit<VertexBuffer_SetLayout>(mHandle, inLayout);
		}
	};

}