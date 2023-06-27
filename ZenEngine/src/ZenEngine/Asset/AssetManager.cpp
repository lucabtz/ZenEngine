#include "AssetManager.h"

#include "ZenEngine/Core/Macros.h"
#include "StaticMesh.h"
#include "ShaderAsset.h"
#include "Texture2DAsset.h"

#include "Serialization.h"

namespace ZenEngine
{
    std::unique_ptr<AssetManager> AssetManager::sAssetManagerInstance;
    static const char *sAssetDirectory = "Assets";

    void AssetManager::Init()
    {    
        RegisterCoreAssets();
        if (!std::filesystem::exists(sAssetDirectory))
            std::filesystem::create_directories(sAssetDirectory);
        BuildAssetDatabase();
    }

    void AssetManager::RegisterAssetClass(const char *inAssetClassName)
    {
        RegisterAssetClass(inAssetClassName, BinarySerializer::GetStaticName());
    }

    void AssetManager::RegisterAssetClass(const char *inAssetClassName, const char *inSerializerName)
    {
        mAssetClasses[inAssetClassName] = inAssetClassName;
        bool found = false;
        mSerializerAssociations[inAssetClassName] = mSerializers[inSerializerName].get();
        if (!found) ZE_ASSERT_CORE_MSG("Serializer {} does not exist!", inSerializerName);
    }

    void AssetManager::RegisterSerializer(std::unique_ptr<AssetSerializer> inSerializer)
    {
        mSerializers[inSerializer->GetName()] = std::move(inSerializer);
    }

    void AssetManager::RegisterImporter(std::unique_ptr<AssetImporter> inImporter)
    {
        mImporters.push_back(std::move(inImporter));
    }

    void AssetManager::Import(const std::filesystem::path &inFilepath)
    {
        Import(inFilepath, sAssetDirectory);
    }

    void AssetManager::Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder)
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
                    mAssetDatabase[importedAsset.Id] = Asset(importedAsset, inDestinationFolder);
                    SaveAsset(importedAsset.Instance, mAssetDatabase[importedAsset.Id]);
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

    const std::unordered_map<UUID, Asset> &AssetManager::GetAssetDatabase() const
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

    std::shared_ptr<AssetInstance> AssetManager::LoadAsset(UUID inUUID)
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

        auto &serializer = mSerializerAssociations[asset.ClassName];
        std::shared_ptr<AssetInstance> assetInstance = serializer->Load(asset);

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
        RegisterSerializer(std::make_unique<BinarySerializer>());
        RegisterSerializer(std::make_unique<ShaderSerializer>());

        REGISTER_NEW_ASSET_CLASS(ZenEngine::StaticMesh);
        RegisterImporter(std::make_unique<OBJImporter>());
        
        REGISTER_NEW_ASSET_CLASS_SERIALIZER(ZenEngine::ShaderAsset, ZenEngine::ShaderSerializer);

        REGISTER_NEW_ASSET_CLASS(ZenEngine::Texture2DAsset);
        RegisterImporter(std::make_unique<PNGImporter>());
    }

    bool AssetManager::SaveAsset(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset)
    {
        auto &serializer = mSerializerAssociations[inAsset.ClassName];
        return serializer->Save(inAssetInstance, inAsset);
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
            for (auto &[_, serializer] : mSerializers)
            {
                if (serializer->CanSerialize(path))
                {
                    auto [id, className] = serializer->GetAssetIdAssetClass(path);
                    Asset asset;
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
    
    void AssetManager::BuildAssetDatabase()
    {
        mAssetDatabase.clear();
        BuildAssetDatabaseFrom(sAssetDirectory);
    }

    bool BinarySerializer::Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const
    {
        if (std::filesystem::exists(inAsset.Filepath))
        {
            ZE_CORE_ERROR("{} already exists!", inAsset.Filepath);
            return false;
        }
        std::ofstream os(inAsset.Filepath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        std::string className = inAssetInstance->GetAssetClassName();
        archive(className, inAsset.Id, inAssetInstance);
        return true;
    }

    std::shared_ptr<AssetInstance> BinarySerializer::Load(const Asset &inAsset) const
    {
        std::ifstream ifs(inAsset.Filepath, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        UUID id;
        std::shared_ptr<AssetInstance> assetInstance;
        std::string className;
        try
        {
            archive(className, id, assetInstance);
        }
        catch (cereal::Exception e)
        {
            ZE_CORE_ERROR("{}", e.what());
            return nullptr;
        }
        SetId(assetInstance, id);
        return assetInstance;
    }

    bool BinarySerializer::CanSerialize(const std::filesystem::path &inFilepath) const
    {
        // TODO think about this
        return (inFilepath.extension().string() == ".zasset");
    }
    
    std::pair<UUID, const char*> BinarySerializer::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        std::ifstream ifs(inFilepath, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        std::string className;
        UUID id;
        archive(className, id);
        return { id, AssetManager::Get().GetAssetClassByName(className) };
    }
}