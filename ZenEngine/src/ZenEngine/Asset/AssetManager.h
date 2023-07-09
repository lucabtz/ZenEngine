#pragma once

#include <optional>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "Asset.h"
#include "AssetLoader.h"
#include "UUID.h"

#include "Serialization.h"
#include "ZenEngine/Core/Log.h"

namespace ZenEngine
{

    struct ImportedAsset
    {
        std::filesystem::path Filename;
        const char *ClassName;
        std::shared_ptr<Asset> Instance;
    };

    struct AssetInfo
    {
        UUID Id;
        std::filesystem::path Filepath;
        const char *ClassName;
       
        AssetInfo() = default;
        AssetInfo(UUID inId, const ImportedAsset &inImportedAsset, const std::filesystem::path &inDestinationPath)
            : Id(inId), Filepath(inDestinationPath / inImportedAsset.Filename), ClassName(inImportedAsset.ClassName)
        {}

        std::string GetName() const { return Filepath.stem().string(); }
    };

    class AssetImporter
    {
    public:
        virtual const char *GetName() const { return "AssetImporter"; }

        virtual std::unordered_set<std::string> GetImportableExtensions() const = 0;
        virtual std::vector<ImportedAsset> Import(const std::filesystem::path &inFilepath) = 0;
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

        std::shared_ptr<Asset> LoadAsset(UUID inUUID);
        bool IsLoaded(UUID inAssetId);
        
        template <IsAsset AssetClass>
        void RegisterAssetClass()
        {
            mAssetClasses[AssetClass::GetStaticAssetClassName()] = AssetClass::GetStaticAssetClassName();
            mAssetLoaders[AssetClass::GetStaticAssetClassName()] = AssetClass::Loader::Get();
        }

        template <typename Importer>
        void RegisterImporter()
        {
            mImporters.push_back(std::make_unique<Importer>());
        }
        
        void Import(const std::filesystem::path &inFilepath);
        void Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder);
        void Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder, UUID inUUID);

        void BuildAssetDatabase();
        const std::unordered_map<UUID, AssetInfo> &GetAssetDatabase() const;
        const AssetInfo &GetAsset(UUID inUUID) const { return mAssetDatabase.at(inUUID); }
        const char *GetAssetClassName(UUID inUUID) const { return mAssetDatabase.at(inUUID).ClassName; }
        bool Exists(UUID inUUID) const { return mAssetDatabase.contains(inUUID); }

        const char *QueryFilepathAssetClassName(const std::filesystem::path &inFilepath) const;
        std::optional<UUID> QueryFilepathAssetId(const std::filesystem::path &inFilepath) const;

        template <IsAsset T>
        std::shared_ptr<T> LoadAssetAs(UUID inUUID)
        {
            return std::static_pointer_cast<T>(LoadAsset(inUUID));
        }

        const char *GetAssetClassByName(const std::string &inName) const { return mAssetClasses.at(inName); }
        bool SaveAsset(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath);
    private:
        std::unordered_map<std::string, const char *> mAssetClasses;
        std::unordered_map<const char *, AssetLoader *> mAssetLoaders;
        std::vector<std::unique_ptr<AssetImporter>> mImporters;

        std::unordered_map<UUID, AssetInfo> mAssetDatabase;
        std::unordered_map<UUID, std::weak_ptr<Asset>> mAssetCache;

        static std::unique_ptr<AssetManager> sAssetManagerInstance;
    
        void RegisterCoreAssets();
        void ImportDefaultAssets();

        void BuildAssetDatabaseFrom(const std::filesystem::path &inImportFolder);
    };
}