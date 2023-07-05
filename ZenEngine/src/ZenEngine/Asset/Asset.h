#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Serialization.h"
#include "UUID.h"

namespace ZenEngine
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
        virtual const char *GetAssetClassName() const = 0;
        UUID GetAssetId() const { return mId; }
    private:
        UUID mId;
        friend class AssetManager;
        friend class AssetLoader;
    };

    template <typename T>
    concept IsAsset = 
    std::derived_from<T, Asset>  
    && requires { { T::GetStaticAssetClassName() } -> std::same_as<const char*>; };

}
#include "AssetManager.h"

#define IMPLEMENT_ASSET_CLASS(name) static const char *GetStaticAssetClassName() { return #name; }\
    virtual const char *GetAssetClassName() const override { return GetStaticAssetClassName(); }\
