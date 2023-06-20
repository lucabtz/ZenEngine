#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "Asset.h"
#include "UUID.h"

#include "Serialization.h"
#include "ZenEngine/Core/Log.h"

namespace ZenEngine
{

    struct AssetClass
    {
        std::string Name;

    private:
        template <typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(Name);
        }

        friend class cereal::access;
    };

    struct Asset
    {
        UUID Id;
        std::filesystem::path Filepath;
        AssetClass Class;
    };

    class AssetImporter
    {
    public:
        virtual const AssetClass &ProvidesAsset() const = 0;
        virtual const char *GetName() const { return "AssetImporter"; }

        virtual std::vector<std::string> ProvidesForExtensions() const = 0;
        virtual std::vector<std::shared_ptr<AssetType>> Import(const std::filesystem::path &inFilepath) = 0;
    };

    template <typename T>
    class AssetImporterFor : public AssetImporter
    {
    public:
        AssetClass const &ProvidesAsset() const override { return T::GetStaticAssetClass(); }
    };
    
    class AssetManager
    {
    public:
        static AssetManager &Get()
        {
            if (sAssetManagerInstance == nullptr) sAssetManagerInstance = std::make_unique<AssetManager>();
            return *sAssetManagerInstance;
        }

        void Init();
        
        void RegisterAssetClass(const AssetClass &inAssetClass);

        void RegisterImporter(std::unique_ptr<AssetImporter> inImporter);

        void Import(const std::filesystem::path &inFilepath);
        void Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder);

        const AssetClass &GetAssetClassByName(const std::string &inName) const;

        bool IsLoaded(UUID inAssetId);

        const std::unordered_map<UUID, Asset> &GetAssetDatabase() const;

        template <typename T>
        std::shared_ptr<T> LoadAsset(UUID inUUID)
        {
            if (!mAssetDatabase.contains(inUUID))
            {
                ZE_CORE_WARN("Asset {} does not exist!", (size_t)inUUID);
                return nullptr;
            }
            if (mAssetCache.contains(inUUID))
            {
                auto weakPtr = mAssetCache[inUUID];
                auto asset = weakPtr.lock();
                if (asset != nullptr)
                {
                    return std::static_pointer_cast<T>(asset);
                }
            }

            auto &assetRes = mAssetDatabase[inUUID];

            if (!std::filesystem::exists(assetRes.Filepath))
            {
                ZE_CORE_WARN("The asset {} does not exist anymore. Maybe it was moved. Rebuilding database", assetRes.Filepath.string());
                BuildAssetDatabase();
                assetRes = mAssetDatabase[inUUID];
                if (!std::filesystem::exists(assetRes.Filepath))
                {
                    ZE_CORE_ERROR("Asset {} has been deleted. Removing it from database.", (uint64_t)inUUID);
                    mAssetCache.erase(inUUID);
                    return nullptr;
                }
            }
            std::ifstream ifs(assetRes.Filepath, std::ios::binary);
            cereal::BinaryInputArchive archive(ifs);
            UUID id;
            std::shared_ptr<AssetType> asset;
            AssetClass clas;
            try
            {
                archive(clas, id, asset);
            }
            catch (cereal::Exception e)
            {
                ZE_CORE_ERROR("{}", e.what());
            }
            mAssetCache[inUUID] = asset;

            if (id != inUUID) ZE_CORE_ERROR("The asset loaded from {} appears to not match UUID. Has the file been tampered with?", assetRes.Filepath);
            return std::static_pointer_cast<T>(asset);
        }

    private:
        std::unordered_map<std::string, AssetClass> mAssetClasses;
        std::vector<std::unique_ptr<AssetImporter>> mImporters;

        std::unordered_map<UUID, Asset> mAssetDatabase;
        std::unordered_map<UUID, std::weak_ptr<AssetType>> mAssetCache;

        static std::unique_ptr<AssetManager> sAssetManagerInstance;
    
        void RegisterCoreAssets();

        bool SaveAsset(const std::shared_ptr<AssetType> &inAssetType, const Asset &inAsset);
        void BuildAssetDatabaseFrom(const std::filesystem::path &inImportFolder);
        void BuildAssetDatabase();
    };



}

#define REGISTER_NEW_ASSET_CLASS(name) {\
    AssetClass ac;\
    ac.Name = #name;\
    AssetManager::Get().RegisterAssetClass(ac);}