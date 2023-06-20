#include "AssetManager.h"

#include "ZenEngine/Core/Macros.h"
#include "StaticMesh.h"

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
        mAssetClasses[inAssetClass.Name] = inAssetClass;
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
                        ZE_CORE_INFO("Imported {} to {} as {} with id {}", inFilepath.string(), asset.Filepath.string(), asset.Class.Name, asset.Id);
                        mAssetDatabase[asset.Id] = asset;
                        ++index;
                    }
                }
                break;
            }
        }
    }

    const AssetClass &AssetManager::GetAssetClassByName(const std::string &inName) const
    {
        ZE_ASSERT_CORE_MSG(mAssetClasses.contains(inName), "Asset class {} does not exist!", inName);
        return mAssetClasses.at(inName);
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

    void AssetManager::RegisterCoreAssets()
    {
        REGISTER_NEW_ASSET_CLASS(ZenEngine::StaticMesh);
        RegisterImporter(std::make_unique<OBJImporter>());
    }

    bool AssetManager::SaveAsset(const std::shared_ptr<AssetType> &inAssetType, const Asset &inAsset)
    {
        if (std::filesystem::exists(inAsset.Filepath))
        {
            ZE_CORE_ERROR("{} already exists!", inAsset.Filepath);
            return false;
        }
        std::ofstream os(inAsset.Filepath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(inAssetType->GetAssetClass(), inAsset.Id, inAssetType);
        return true;
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
            if (path.extension().string() == ".zasset")
            {
                std::ifstream ifs(path, std::ios::binary);
                cereal::BinaryInputArchive archive(ifs);
                Asset assetRes;
                assetRes.Filepath = path;
                archive(assetRes.Class, assetRes.Id);
                if (mAssetDatabase.contains(assetRes.Id))
                {
                    ZE_CORE_WARN("An asset with the same id already exists. Is it being imported twice?");
                }
                mAssetDatabase[assetRes.Id] = assetRes;
                ZE_CORE_TRACE("Loaded {} asset {} into database", assetRes.Class.Name, (uint64_t)assetRes.Id);
            }
        }
    }
    void AssetManager::BuildAssetDatabase()
    {
        BuildAssetDatabaseFrom(sAssetDirectory);
    }
}