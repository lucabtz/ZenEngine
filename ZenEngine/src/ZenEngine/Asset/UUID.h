#pragma once

#include <cstdint>
#include "Serialization.h"

namespace ZenEngine
{

    class UUID
    {
    public:
        UUID();
        UUID(uint64_t inUUID);
        UUID(const UUID&) = default;
 
        operator uint64_t() const { return mUUID; }
        //bool operator ==(const UUID &inOther) { return mUUID == inOther.mUUID; }
    private:
        uint64_t mUUID;

        template<typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(mUUID); 
        }

        friend class cereal::access;
    };

}

namespace std 
{
    template <typename T> struct hash;

    template<>
    struct hash<ZenEngine::UUID>
    {
        size_t operator()(const ZenEngine::UUID& inUUID) const
        {
            return (uint64_t)inUUID;
        }
    };

}