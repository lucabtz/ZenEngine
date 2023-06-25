#pragma once

#include <optional>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "AssetInstance.h"
#include "UUID.h"

#include "Serialization.h"
#include "ZenEngine/Core/Log.h"

namespace ZenEngine
{

    struct ImportedAsset
    {
        UUID Id;
        std::filesystem::path Filename;
        const char *ClassName;
        std::shared_ptr<AssetInstance> Instance;
    };

    struct Asset
    {
        UUID Id;
        std::filesystem::path Filepath;
        const char *ClassName;
       
        Asset() = default;
        Asset(const ImportedAsset &inImportedAsset, const std::filesystem::path &inDestinationPath)
            : Id(inImportedAsset.Id), Filepath(inDestinationPath / inImportedAsset.Filename), ClassName(inImportedAsset.ClassName)
        {}
    };

    class AssetImporter
    {
    public:
        virtual const char *GetName() const { return "AssetImporter"; }

        virtual std::unordered_set<std::string> GetImportableExtensions() const = 0;
        virtual std::vector<ImportedAsset> Import(const std::filesystem::path &inFilepath) = 0;
    };

    class AssetSerializer
    {
    public:
        virtual const char *GetName() const = 0;
        virtual bool Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const  = 0;
        virtual std::shared_ptr<AssetInstance> Load(const Asset &inAsset) const = 0;
        virtual bool CanSerialize(const std::filesystem::path &inFilepath) const = 0;
        virtual std::pair<UUID, const char*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const = 0;
    protected:
        void SetId(std::shared_ptr<AssetInstance> &inAssetInstance, UUID inUUID) const { inAssetInstance->mId = inUUID; }
    };

#define IMPLEMENT_SERIALIZER_CLASS(fqname) static const char *GetStaticName() { return #fqname; }\
    virtual const char *GetName() const { return GetStaticName(); }

    class BinarySerializer : public AssetSerializer
    {
    public:
        IMPLEMENT_SERIALIZER_CLASS(ZenEngine::BinarySerializer)

        virtual bool Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const override;
        virtual std::shared_ptr<AssetInstance> Load(const Asset &inAsset) const override;
        virtual bool CanSerialize(const std::filesystem::path &inFilepath) const override;
        virtual std::pair<UUID, const char*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const override;
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

        std::shared_ptr<AssetInstance> LoadAsset(UUID inUUID);
        bool IsLoaded(UUID inAssetId);
        
        void RegisterAssetClass(const char *inAssetClass);
        void RegisterAssetClass(const char *inAssetClass, const char *inSerializerName);
        void RegisterSerializer(std::unique_ptr<AssetSerializer> inSerializer);
        void RegisterImporter(std::unique_ptr<AssetImporter> inImporter);

        void Import(const std::filesystem::path &inFilepath);
        void Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder);

        void BuildAssetDatabase();
        const std::unordered_map<UUID, Asset> &GetAssetDatabase() const;
        const char *GetAssetClassName(UUID inUUID) const { return mAssetDatabase.at(inUUID).ClassName; }

        const char *QueryFilepathAssetClassName(const std::filesystem::path &inFilepath) const;
        std::optional<UUID> QueryFilepathAssetId(const std::filesystem::path &inFilepath) const;

        template <IsAssetInstance T>
        std::shared_ptr<T> LoadAssetAs(UUID inUUID)
        {
            return std::static_pointer_cast<T>(LoadAsset(inUUID));
        }

        const char *GetAssetClassByName(const std::string &inName) const { return mAssetClasses.at(inName); }
    private:
        std::unordered_map<std::string, const char *> mAssetClasses;
        std::unordered_map<const char *, std::unique_ptr<AssetSerializer>> mSerializers;
        std::vector<std::unique_ptr<AssetImporter>> mImporters;

        std::unordered_map<UUID, Asset> mAssetDatabase;
        std::unordered_map<UUID, std::weak_ptr<AssetInstance>> mAssetCache;
        std::unordered_map<const char *, AssetSerializer *> mSerializerAssociations;

        static std::unique_ptr<AssetManager> sAssetManagerInstance;
    
        void RegisterCoreAssets();

        bool SaveAsset(const std::shared_ptr<AssetInstance> &inAssetType, const Asset &inAsset);
        void BuildAssetDatabaseFrom(const std::filesystem::path &inImportFolder);
    };



}

#define REGISTER_NEW_ASSET_CLASS(name) {\
    AssetManager::Get().RegisterAssetClass(name::GetStaticAssetClassName());}

#define REGISTER_NEW_ASSET_CLASS_SERIALIZER(name, serializer) {\
    AssetManager::Get().RegisterAssetClass(name::GetStaticAssetClassName(), serializer::GetStaticName());}