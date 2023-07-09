#include "AssetLoader.h"

#include "Serialization.h"

namespace ZenEngine
{
    std::shared_ptr<Asset> BinaryLoader::Load(const std::filesystem::path &inFilepath) const
    {
        std::ifstream ifs(inFilepath, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        UUID id;
        std::shared_ptr<Asset> assetInstance;
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

    bool BinaryLoader::Save(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath) const
    {
        if (std::filesystem::exists(inFilepath))
        {
            ZE_CORE_ERROR("{} already exists!", inFilepath);
            return false;
        }
        std::ofstream os(inFilepath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        std::string className = inAssetInstance->GetAssetClassName();
        archive(className, inAssetInstance->GetAssetId(), inAssetInstance);
        return true;
    }

    bool BinaryLoader::CanLoad(const std::filesystem::path &inFilepath) const
    {
        // TODO think about this
        return (inFilepath.extension().string() == ".zasset");
    }
    
    std::pair<UUID, const char*> BinaryLoader::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        std::ifstream ifs(inFilepath, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        std::string className;
        UUID id;
        archive(className, id);
        return { id, AssetManager::Get().GetAssetClassByName(className) };
    }
}