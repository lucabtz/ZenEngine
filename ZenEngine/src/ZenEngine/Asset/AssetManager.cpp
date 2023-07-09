#include "AssetManager.h"

#include "ZenEngine/Core/Macros.h"
#include "StaticMesh.h"
#include "ShaderAsset.h"
#include "Texture2DAsset.h"

#include "Serialization.h"

namespace ZenEngine
{
    std::vector<AssetLoader *> AssetLoader::sAllLoaders;
    std::unique_ptr<AssetManager> AssetManager::sAssetManagerInstance;
    static const char *sAssetDirectory = "Assets";

    void AssetManager::Init()
    {    
        RegisterCoreAssets();
        if (!std::filesystem::exists(sAssetDirectory))
            std::filesystem::create_directories(sAssetDirectory);

        if (!std::filesystem::exists(std::filesystem::path(sAssetDirectory) / "EngineAssets"))
        {
            std::filesystem::create_directories(std::filesystem::path(sAssetDirectory) / "EngineAssets");
            ImportDefaultAssets();
        }
        BuildAssetDatabase();
    }

    void AssetManager::Import(const std::filesystem::path &inFilepath)
    {
        Import(inFilepath, sAssetDirectory);
    }

    void AssetManager::Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder)
    {
        Import(inFilepath, inDestinationFolder, UUID());
    }

    void AssetManager::Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder, UUID inUUID)
    {
        ZE_CORE_INFO("Importing {} to {}", inFilepath.string(), inDestinationFolder.string());

        auto extension = inFilepath.extension().string();
        std::string filename = inFilepath.stem().string();
        std::filesystem::path destination(inDestinationFolder);
        
        if (!std::filesystem::exists(destination))
            std::filesystem::create_directories(destination);
        
        for (auto &importer : mImporters)
        {
            ZE_CORE_TRACE("Trying importer {}", importer->GetName());
            auto provided = importer->GetImportableExtensions();
            if (provided.contains(extension))
            {
                ZE_CORE_INFO("Found importer {}", importer->GetName());
                uint32_t index = 0;
                for (auto &importedAsset : importer->Import(inFilepath))
                {
                    mAssetDatabase[inUUID] = AssetInfo(inUUID, importedAsset, inDestinationFolder);
                    SaveAsset(importedAsset.Instance, mAssetDatabase[inUUID].Filepath);
                }
                break;
            }
        }
    }

    bool AssetManager::IsLoaded(UUID inAssetId)
    {
        if (!mAssetCache.contains(inAssetId)) return false;
        return !mAssetCache[inAssetId].expired();
    }

    const std::unordered_map<UUID, AssetInfo> &AssetManager::GetAssetDatabase() const
    {
        return mAssetDatabase;
    }

    const char *AssetManager::QueryFilepathAssetClassName(const std::filesystem::path &inFilepath) const
    {
        for (auto &[uuid, asset] : mAssetDatabase)
        {
            if (asset.Filepath == inFilepath)
            {
                return asset.ClassName;
            }
        }
        return nullptr;
    }

    std::optional<UUID> AssetManager::QueryFilepathAssetId(const std::filesystem::path &inFilepath) const
    {
        for (auto &[uuid, asset] : mAssetDatabase)
        {
            if (asset.Filepath == inFilepath)
            {
                return uuid;
            }
        }
        return std::nullopt;
    }

    std::shared_ptr<Asset> AssetManager::LoadAsset(UUID inUUID)
    {
        if (!mAssetDatabase.contains(inUUID))
        {
            ZE_CORE_WARN("Asset {} does not exist!", (size_t)inUUID);
            return nullptr;
        }
        if (mAssetCache.contains(inUUID))
        {
            auto weakPtr = mAssetCache[inUUID];
            auto assetInstance = weakPtr.lock();
            if (assetInstance != nullptr)
            {
                return assetInstance;
            }
        }

        auto &asset = mAssetDatabase[inUUID];

        // TODO maybe refactor this
        if (!std::filesystem::exists(asset.Filepath))
        {
            ZE_CORE_WARN("The asset {} does not exist anymore. Maybe it was moved. Rebuilding database", asset.Filepath.string());
            BuildAssetDatabase();
            asset = mAssetDatabase[inUUID];
            if (!std::filesystem::exists(asset.Filepath))
            {
                ZE_CORE_ERROR("Asset {} has been deleted. Removing it from database.", (uint64_t)inUUID);
                mAssetCache.erase(inUUID);
                return nullptr;
            }
        }

        auto &loader = mAssetLoaders[asset.ClassName];
        std::shared_ptr<Asset> assetInstance = loader->Load(asset.Filepath);

        if (assetInstance == nullptr)
        {
            ZE_CORE_ERROR("Could not load asset {}", (uint64_t)inUUID);
            return nullptr;
        }

        mAssetCache[inUUID] = assetInstance;

        if (assetInstance->GetAssetId() != inUUID) ZE_CORE_ERROR("The asset loaded from {} appears to not match UUID. Has the file been tampered with?", asset.Filepath);
        // TODO: if for some reason the id has changed we should save the asset with the new id?

        return assetInstance;
    }

    void AssetManager::RegisterCoreAssets()
    {
        RegisterAssetClass<StaticMesh>();
        RegisterImporter<OBJImporter>();

        RegisterAssetClass<ShaderAsset>();
        
        RegisterAssetClass<Texture2DAsset>();
        RegisterImporter<STBImageImporter>();
    }

    bool AssetManager::SaveAsset(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath)
    {
        auto &loader = mAssetLoaders[inAssetInstance->GetAssetClassName()];
        return loader->Save(inAssetInstance, inFilepath);
    }

    void AssetManager::BuildAssetDatabaseFrom(const std::filesystem::path &inImportFolder)
    {
        if (!std::filesystem::is_directory(inImportFolder))
        {
            ZE_CORE_ERROR("Asset import folder should be a directory");
            return;
        }
        for (auto &dirEntry : std::filesystem::directory_iterator(inImportFolder))
        {
            auto path = dirEntry.path();
            if (std::filesystem::is_directory(path))
            {
                BuildAssetDatabaseFrom(path);
                continue;
            }
            for (auto &loader : AssetLoader::GetAllLoaders())
            {
                if (loader->CanLoad(path))
                {
                    auto [id, className] = loader->GetAssetIdAssetClass(path);
                    AssetInfo asset;
                    asset.ClassName = className;
                    asset.Id = id;
                    asset.Filepath = path;
                    if (mAssetDatabase.contains(asset.Id))
                    {
                        ZE_CORE_WARN("An asset with the same id already exists. Is it being imported twice?");
                    }
                    mAssetDatabase[id] = asset;
                    ZE_CORE_TRACE("Loaded {} asset {} into database", asset.ClassName, (uint64_t)asset.Id);
                }
            }
        }
    }

    void AssetManager::ImportDefaultAssets()
    {
        // import default texture
        Import("resources/Textures/DefaultTexture.png", std::filesystem::path(sAssetDirectory) / "EngineAssets", 1);
    }
    
    void AssetManager::BuildAssetDatabase()
    {
        mAssetDatabase.clear();
        BuildAssetDatabaseFrom(sAssetDirectory);
    }

}