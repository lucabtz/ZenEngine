#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Serialization.h"
#include "UUID.h"

namespace ZenEngine
{
    class AssetInstance
    {
    public:
        virtual ~AssetInstance() = default;
        virtual const char *GetAssetClassName() const = 0;
        UUID GetAssetId() const { return mId; }
    private:
        UUID mId;
        friend class AssetManager;
        friend class AssetSerializer;
    };

    template <typename T>
    concept IsAssetInstance = std::derived_from<T, AssetInstance> && !std::is_abstract_v<T> && requires { { T::GetStaticAssetClassName() } -> std::same_as<const char*>; };
}
#include "AssetManager.h"

#define IMPLEMENT_ASSET_CLASS(name) static const char *GetStaticAssetClassName() { return #name; }\
    virtual const char *GetAssetClassName() const override { return GetStaticAssetClassName(); }\
    friend class Asset;
