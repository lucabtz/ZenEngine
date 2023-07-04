#pragma once

#include <memory>

namespace ZenEngine
{
    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() {}

        virtual void Bind() = 0;
        virtual void Bind(uint32_t inBinding) = 0;
        virtual void SetData(const void* inData, uint32_t inSize, uint32_t inOffset = 0) = 0;
        
        static std::shared_ptr<UniformBuffer> Create(uint32_t inSize, uint32_t inBinding);
    };
}

#include "ZenEngine/Core/Macros.h"

#define UB_STRUCT_MAT4(structname, membername)  static_assert(offsetof(structname, membername) % 16 == 0, "Invalid alignment")
#define UB_STRUCT_VEC4(structname, membername)  static_assert(offsetof(structname, membername) % 16 == 0, "Invalid alignment")
#define UB_STRUCT_VEC3(structname, membername)  static_assert(offsetof(structname, membername) % 16 == 0, "Invalid alignment")
#define UB_STRUCT_FLOAT(structname, membername) static_assert(offsetof(structname, membername) % 4 == 0, "Invalid alignment")

#define UB_STRUCT_PADDING(padding) float COMBINE(Padding, __LINE__) ## [padding]
