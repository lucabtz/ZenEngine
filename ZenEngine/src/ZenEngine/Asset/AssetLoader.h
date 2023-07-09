#pragma once

#include "Asset.h"
#include <memory>
#include <vector>
#include <filesystem>

namespace ZenEngine
{
    struct AssetInfo;

    class AssetLoader
    {
    public:
        virtual ~AssetLoader() = default;
        virtual const char *GetName() const = 0;
        virtual bool Save(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath) const  = 0;
        virtual std::shared_ptr<Asset> Load(const std::filesystem::path &inFilepath) const = 0;
        virtual bool CanLoad(const std::filesystem::path &inFilepath) const = 0;
        virtual std::pair<UUID, const char*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const = 0;

        static const std::vector<AssetLoader *> &GetAllLoaders() { return sAllLoaders; }
    protected:
        void SetId(std::shared_ptr<Asset> &inAssetInstance, UUID inUUID) const { inAssetInstance->mId = inUUID; }
        
        static std::vector<AssetLoader *> sAllLoaders;
    };

#define IMPLEMENT_LOADER_CLASS(fqname) static const char *GetStaticName() { return #fqname; }\
    virtual const char *GetName() const { return GetStaticName(); }\
    static AssetLoader *Get() \
    {\
        static std::unique_ptr<AssetLoader> instance;\
        if (instance == nullptr) { instance = std::make_unique<:: ##fqname>(); sAllLoaders.push_back(instance.get()); }\
        return instance.get();\
    }

    struct AssetSystemMeta
    {
        UUID Id;
        std::string ClassName;

    private:
        template <typename Archive>
        void Serialize(Archive &outArchive)
        {
            outArchive(cereal::make_nvp("id", Id), cereal::make_nvp("class", ClassName));
        }
        friend class cereal::access;
    };

    template <typename ClientMeta>
    struct Meta
    {
        AssetSystemMeta AssetMeta;
        ClientMeta AdditionalMeta;
    private:
        template <typename Archive>
        void Serialize(Archive &outArchive)
        {
            outArchive(cereal::make_nvp("asset", AssetMeta), cereal::make_nvp("client", AdditionalMeta));
        }
        friend class cereal::access;
    };

    struct EmptyClientMeta
    {
    private:
        template <typename Archive>
        void Serialize(Archive &outArchive) {}
        friend class cereal::access;
    };

    /// @brief A meta loader stores asset info such as class and UUID in a separate .zmeta file plus eventual client metadata
    template <typename ClientMeta>
    class ClientMetaLoader : public AssetLoader
    {
    public:
        bool SaveMeta(const std::shared_ptr<Asset> &inAssetInstance, const ClientMeta &inClientMeta, const std::filesystem::path &inFilepath) const;
        Meta<ClientMeta> LoadMeta(const std::filesystem::path &inFilepath) const;
        bool DoesMetaFileExist(const std::filesystem::path &inFilepath) const;
        virtual std::pair<UUID, const char*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const override;
    };

    using MetaLoader = ClientMetaLoader<EmptyClientMeta>;

    class BinaryLoader : public AssetLoader
    {
    public:
        IMPLEMENT_LOADER_CLASS(ZenEngine::BinaryLoader)

        virtual bool Save(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath) const override;
        virtual std::shared_ptr<Asset> Load(const std::filesystem::path &inFilepath) const override;
        virtual bool CanLoad(const std::filesystem::path &inFilepath) const override;
        virtual std::pair<UUID, const char*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const override;
    };

    template <typename ClientMeta>
    inline bool ClientMetaLoader<ClientMeta>::SaveMeta(const std::shared_ptr<Asset> &inAssetInstance, const ClientMeta &inClientMeta, const std::filesystem::path &inFilepath) const
    {
        auto metaPath = (inFilepath.parent_path())/inFilepath.filename().replace_extension(".zmeta");
        std::ofstream ofs(metaPath);
        cereal::JSONOutputArchive archive(ofs);
        Meta<ClientMeta> metadata{};
        metadata.AdditionalMeta = inClientMeta;
        metadata.AssetMeta.Id = inAssetInstance->GetAssetId();
        metadata.AssetMeta.ClassName = inAssetInstance->GetAssetClassName();
        archive(metadata);
        return true;
    }

    template <typename ClientMeta>
    inline Meta<ClientMeta> ClientMetaLoader<ClientMeta>::LoadMeta(const std::filesystem::path &inFilepath) const
    {
        auto metaPath = (inFilepath.parent_path())/inFilepath.filename().replace_extension(".zmeta");
        std::ifstream ifs(metaPath);
        cereal::JSONInputArchive archive(ifs);
        Meta<ClientMeta> metadata{};
        archive(metadata);
        return metadata;
    }

    template <typename ClientMeta>
    inline bool ClientMetaLoader<ClientMeta>::DoesMetaFileExist(const std::filesystem::path &inFilepath) const
    {
        return std::filesystem::exists((inFilepath.parent_path())/inFilepath.filename().replace_extension(".zmeta"));
    }

    template <typename ClientMeta>
    inline std::pair<UUID, const char *> ClientMetaLoader<ClientMeta>::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        Meta<ClientMeta> metadata = LoadMeta(inFilepath);
        return { metadata.AssetMeta.Id, AssetManager::Get().GetAssetClassByName(metadata.AssetMeta.ClassName) };
    }
}