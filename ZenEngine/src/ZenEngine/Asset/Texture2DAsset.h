#pragma once

#include "Asset.h"
#include "ZenEngine/Renderer/Texture2D.h"

namespace cereal
{
    template <typename Archive>
    void Serialize(Archive &inArchive, ZenEngine::Texture2D::Properties &inProps)
    {
        inArchive(inProps.Width, inProps.Height, inProps.Format, inProps.MagFilter, inProps.MagFilter, inProps.GenerateMips);
    }
}

namespace ZenEngine
{
    class Texture2DAsset : public Asset
    {
    public:
        IMPLEMENT_ASSET_CLASS(ZenEngine::Texture2DAsset)
        using Loader = BinaryLoader;

        std::shared_ptr<Texture2D> CreateOrGetTexture2D();

        const Texture2D::Properties &GetTextureProperties() const { return mTextureProperties; }

        void SetTextureProperties(const Texture2D::Properties &inTextureProperties) { mTextureProperties = inTextureProperties; mTainted = true; }
        void SetData(std::vector<uint8_t> inData) { mData = std::move(inData); mTainted = true; }

    private:
        Texture2D::Properties mTextureProperties;
        std::vector<uint8_t> mData;

        bool mTainted = false;
        std::shared_ptr<Texture2D> mTexture2D;

        template <typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(mTextureProperties, mData);
        }

        friend class cereal::access;
    };

    class STBImageImporter : public AssetImporter
    {
        public:
        virtual const char *GetName() const { return "STBImageImporter"; }
        virtual std::unordered_set<std::string> GetImportableExtensions() const 
        {
            return { ".png" };
        }

        virtual std::vector<ImportedAsset> Import(const std::filesystem::path &inFilepath) override;
    };
}

CEREAL_REGISTER_TYPE(ZenEngine::Texture2DAsset);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ZenEngine::Asset, ZenEngine::Texture2DAsset)