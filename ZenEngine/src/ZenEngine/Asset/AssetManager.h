#pragma once

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
    struct AssetClass
    {
        std::string Name;

        class AssetSerializer *Serializer;
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
        const AssetClass *Class;
    };

    class AssetImporter
    {
    public:
        virtual const AssetClass *ProvidesAsset() const = 0;
        virtual const char *GetName() const { return "AssetImporter"; }

        virtual std::vector<std::string> ProvidesForExtensions() const = 0;
        virtual std::vector<std::shared_ptr<AssetInstance>> Import(const std::filesystem::path &inFilepath) = 0;
    };

    template <typename T>
    class AssetImporterFor : public AssetImporter
    {
    public:
        const AssetClass *ProvidesAsset() const override { return T::GetStaticAssetClass(); }
    };
    

    class AssetSerializer
    {
    public:
        virtual const char *GetName() const = 0;
        virtual bool Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const  = 0;
        virtual std::shared_ptr<AssetInstance> Load(const Asset &inAsset) const = 0;
        virtual bool CanSerialize(const std::filesystem::path &inFilepath) const = 0;
        virtual std::pair<UUID, const AssetClass*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const = 0;
    protected:
        void SetId(std::shared_ptr<AssetInstance> &inAssetInstance, UUID inUUID) const { inAssetInstance->mId = inUUID; }
    };

#define IMPLEMENT_SERIALIZER_CLASS(fqname) virtual const char *GetName() const { return #fqname; }

    class BinarySerializer : public AssetSerializer
    {
    public:
        IMPLEMENT_SERIALIZER_CLASS(ZenEngine::BinarySerializer)

        virtual bool Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const override;
        virtual std::shared_ptr<AssetInstance> Load(const Asset &inAsset) const override;
        virtual bool CanSerialize(const std::filesystem::path &inFilepath) const override;
        virtual std::pair<UUID, const AssetClass*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const override;
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

        const AssetClass *GetAssetClassByName(const std::string &inName) const;
        std::shared_ptr<AssetInstance> LoadAsset(UUID inUUID);
        bool IsLoaded(UUID inAssetId);
        
        void RegisterAssetClass(const AssetClass &inAssetClass);
        void RegisterAssetClass(const AssetClass &inAssetClass, const std::string &inSerializerName);
        void RegisterSerializer(std::unique_ptr<AssetSerializer> inSerializer);
        void RegisterImporter(std::unique_ptr<AssetImporter> inImporter);

        void Import(const std::filesystem::path &inFilepath);
        void Import(const std::filesystem::path &inFilepath, const std::filesystem::path &inDestinationFolder);

        void BuildAssetDatabase();
        const std::unordered_map<UUID, Asset> &GetAssetDatabase() const;

        template <typename T>
        std::shared_ptr<T> LoadAssetAs(UUID inUUID)
        {
            return std::static_pointer_cast<T>(LoadAsset(inUUID));
        }

    private:
        std::unordered_map<std::string, AssetClass> mAssetClasses;
        std::vector<std::unique_ptr<AssetSerializer>> mSerializers;
        std::vector<std::unique_ptr<AssetImporter>> mImporters;

        std::unordered_map<UUID, Asset> mAssetDatabase;
        std::unordered_map<UUID, std::weak_ptr<AssetInstance>> mAssetCache;

        static std::unique_ptr<AssetManager> sAssetManagerInstance;
    
        void RegisterCoreAssets();

        bool SaveAsset(const std::shared_ptr<AssetInstance> &inAssetType, const Asset &inAsset);
        void BuildAssetDatabaseFrom(const std::filesystem::path &inImportFolder);
    };



}

#define REGISTER_NEW_ASSET_CLASS(name) {\
    AssetClass ac;\
    ac.Name = #name;\
    AssetManager::Get().RegisterAssetClass(ac);}

#define REGISTER_NEW_ASSET_CLASS_SERIALIZER(name, serializer) {\
    AssetClass ac;\
    ac.Name = #name;\
    AssetManager::Get().RegisterAssetClass(ac, #serializer);}