#include "AssetManager.h"

#include "ZenEngine/Core/Macros.h"
#include "StaticMesh.h"
#include "ShaderAsset.h"

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

    void AssetManager::RegisterAssetClass(const AssetClass &inAssetClass)
    {
        RegisterAssetClass(inAssetClass, "ZenEngine::BinarySerializer");
    }

    void AssetManager::RegisterAssetClass(const AssetClass &inAssetClass, const std::string &inSerializerName)
    {
        AssetClass ac = inAssetClass;
        bool found = false;
        for (auto &serializer : mSerializers)
        {
            if (serializer->GetName() == inSerializerName)
            {
                ac.Serializer = serializer.get();
                found = true;
            }
        }
        if (!found) ZE_ASSERT_CORE_MSG("Serializer {} does not exist!", inSerializerName);
        mAssetClasses[ac.Name] = ac;
    }

    void AssetManager::RegisterSerializer(std::unique_ptr<AssetSerializer> inSerializer)
    {
        mSerializers.push_back(std::move(inSerializer));
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
            auto provided = importer->ProvidesForExtensions();
            if (std::find(provided.begin(), provided.end(), extension) != provided.end())
            {
                ZE_CORE_INFO("Found importer {}", importer->GetName());
                uint32_t index = 0;
                for (auto &assetType : importer->Import(inFilepath))
                {
                    Asset asset;
                    asset.Filepath = (destination / (index == 0? filename + ".zasset" : fmt::format("{}_{}.zasset", filename, index)));
                    asset.Class = assetType->GetAssetClass();
                    if (SaveAsset(assetType, asset))
                    {
                        ZE_CORE_INFO("Imported {} to {} as {} with id {}", inFilepath.string(), asset.Filepath.string(), asset.Class->Name, asset.Id);
                        mAssetDatabase[asset.Id] = asset;
                        ++index;
                    }
                }
                break;
            }
        }
    }

    const AssetClass *AssetManager::GetAssetClassByName(const std::string &inName) const
    {
        ZE_ASSERT_CORE_MSG(mAssetClasses.contains(inName), "Asset class {} does not exist!", inName);
        return &mAssetClasses.at(inName);
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

    const AssetClass *AssetManager::QueryFilepathAssetClass(const std::filesystem::path &inFilepath) const
    {
        for (auto &[uuid, asset] : mAssetDatabase)
        {
            if (asset.Filepath == inFilepath)
            {
                return asset.Class;
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
            auto asset = weakPtr.lock();
            if (asset != nullptr)
            {
                return asset;
            }
        }

        auto &assetRes = mAssetDatabase[inUUID];

        // TODO maybe refactor this
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

        auto &serializer = assetRes.Class->Serializer;
        std::shared_ptr<AssetInstance> asset = serializer->Load(assetRes);

        if (asset == nullptr)
        {
            ZE_CORE_ERROR("Could not load asset {}", (uint64_t)inUUID);
            return nullptr;
        }

        mAssetCache[inUUID] = asset;

        if (asset->GetAssetId() != inUUID) ZE_CORE_ERROR("The asset loaded from {} appears to not match UUID. Has the file been tampered with?", assetRes.Filepath);
        // TODO: if for some reason the id has changed we should save the asset with the new id?

        return asset;
    }

    void AssetManager::RegisterCoreAssets()
    {
        RegisterSerializer(std::make_unique<BinarySerializer>());
        RegisterSerializer(std::make_unique<ShaderSerializer>());

        REGISTER_NEW_ASSET_CLASS(ZenEngine::StaticMesh);
        RegisterImporter(std::make_unique<OBJImporter>());
        
        REGISTER_NEW_ASSET_CLASS_SERIALIZER(ZenEngine::ShaderAsset, ZenEngine::ShaderSerializer);
    }

    bool AssetManager::SaveAsset(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset)
    {
        auto &serializer = inAsset.Class->Serializer;
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
            for (auto &serializer : mSerializers)
            {
                if (serializer->CanSerialize(path))
                {
                    auto [id, clas] = serializer->GetAssetIdAssetClass(path);
                    Asset asset;
                    asset.Class = clas;
                    asset.Id = id;
                    asset.Filepath = path;
                    if (mAssetDatabase.contains(asset.Id))
                    {
                        ZE_CORE_WARN("An asset with the same id already exists. Is it being imported twice?");
                    }
                    mAssetDatabase[id] = asset;
                    ZE_CORE_TRACE("Loaded {} asset {} into database", asset.Class->Name, (uint64_t)asset.Id);
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
        archive(inAssetInstance->GetAssetClass()->Name, inAsset.Id, inAssetInstance);
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
    
    std::pair<UUID, const AssetClass*> BinarySerializer::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        std::ifstream ifs(inFilepath, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        std::string className;
        UUID id;
        archive(className, id);
        return { id, AssetManager::Get().GetAssetClassByName(className) };
    }
}