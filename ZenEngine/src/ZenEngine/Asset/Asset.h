#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Serialization.h"
#include "UUID.h"

namespace ZenEngine
{
    class AssetType
    {
    public:
        virtual const class AssetClass &GetAssetClass() const = 0;
    };

}
#include "AssetManager.h"

#define IMPLEMENT_ASSET_CLASS(name) static const AssetClass &GetStaticAssetClass() { return AssetManager::Get().GetAssetClassByName(#name); }\
    virtual const AssetClass &GetAssetClass() const override { return GetStaticAssetClass(); }\
    friend class Asset;
