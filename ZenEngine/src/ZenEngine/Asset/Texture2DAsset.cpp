#include "Texture2DAsset.h"

#include <stb_image.h>

namespace ZenEngine
{
    std::shared_ptr<Texture2D> Texture2DAsset::CreateOrGetTexture2D()
    {
        if (mTexture2D != nullptr && !mTainted) return mTexture2D;
        mTexture2D = Texture2D::Create(mTextureProperties);
        mTexture2D->SetData(mData.data());
        mTainted = false;
        return mTexture2D;
    }

    std::vector<ImportedAsset> PNGImporter::Import(const std::filesystem::path &inFilepath)
    {
        int width;
        int height;
        int channels;
        uint8_t *imageData = stbi_load(inFilepath.string().c_str(), &width, &height, &channels, 4);
        Texture2D::Properties props;
        props.Width = width;
        props.Height = height;
        props.MagFilter = Texture2D::Filter::Linear;
        props.MinFilter = Texture2D::Filter::Linear;
        switch (channels)
        {
        case 4: props.Format = Texture2D::Format::RGBA8; break;
        case 3: props.Format = Texture2D::Format::RGB8; break;
        case 1: props.Format = Texture2D::Format::R8; break;
        default: ZE_ASSERT_CORE_MSG(false, "Number of channels not supported");
        }
        std::vector<uint8_t> data(width * height * channels);
        memcpy(data.data(), imageData, width * height * channels);
        std::shared_ptr<Texture2DAsset> textureAsset = std::make_shared<Texture2DAsset>();
        textureAsset->SetTextureProperties(props);
        textureAsset->SetData(std::move(data));

        ImportedAsset imported;
        imported.ClassName = Texture2DAsset::GetStaticAssetClassName();
        imported.Filename = inFilepath.stem();
        imported.Instance = textureAsset;

        return { imported };
    }
}