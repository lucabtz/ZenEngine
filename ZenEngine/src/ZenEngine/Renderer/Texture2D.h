#pragma once

#include <stdint.h>
#include <string>
#include <memory>

namespace ZenEngine
{

	class Texture2D
	{
	public:
        enum class Format
        {
            None = 0,
            R8,
            RGB8,
            RGBA8,
            RGBA32F
        };

        enum class Filter
        {
            None = 0,
            Linear,
            Nearest
        };

        struct Properties
        {
            uint32_t Width = 1;
            uint32_t Height = 1;
            Texture2D::Format Format = Texture2D::Format::RGBA8;
            bool GenerateMips = true;
            Texture2D::Filter MagFilter = Texture2D::Filter::Linear;
            Texture2D::Filter MinFilter = Texture2D::Filter::Linear;
        };

		virtual ~Texture2D() = default;

		virtual const Texture2D::Properties& GetProperties() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* inData, uint32_t inSize) = 0;

		virtual void Bind(uint32_t inSlot = 0) const = 0;

        static std::shared_ptr<Texture2D> Create(const Texture2D::Properties& inProperties);
        static std::shared_ptr<Texture2D> Create(const Texture2D::Properties& inProperties, void *inData, uint32_t inSize);
	};

}