#include "ShaderAsset.h"

#include "ZenEngine/Core/Filesystem.h"
#include "ZenEngine/ShaderCompiler/ShaderCompiler.h"

namespace ZenEngine
{
    std::shared_ptr<Shader> ShaderAsset::CreateOrGetShaderProgram()
    {
        if (mShaderProgram == nullptr || mTainted) 
        {
            mShaderProgram = Shader::Create(mName, mSourceCode);
            mTainted = false;
        }
        return mShaderProgram;
    }

    Shader::ShaderUniformInfo ShaderAsset::GetShaderUniforms()
    {
        auto shader = CreateOrGetShaderProgram();
        return shader->GetShaderUniformInfo();
    }
    
    bool ShaderLoader::Save(const std::shared_ptr<Asset> &inAssetInstance, const AssetInfo &inAssetInfo) const
    {
        ZE_ASSERT_CORE_MSG(false, "ShaderSerializer::Save is not supposed to be called since there are no shader importers! Shaders use a text format an can just be copied in the assets directory.");
        return false;
    }
    
    std::shared_ptr<Asset> ShaderLoader::Load(const AssetInfo &inAssetInfo) const
    {
        std::shared_ptr<ShaderAsset> shader = std::make_unique<ShaderAsset>();
        shader->SetName(inAssetInfo.Filepath.stem().string());
        shader->SetSourceCode(Filesystem::ReadFileToString(inAssetInfo.Filepath));
        auto asset = std::static_pointer_cast<Asset>(shader);
        SetId(asset, GetAssetId(inAssetInfo.Filepath));
        return asset;
    }

    bool ShaderLoader::CanLoad(const std::filesystem::path &inFilepath) const
    {
        return (inFilepath.extension().string() == ".zshader" || inFilepath.extension().string() == ".hlsl");
    }

    std::pair<UUID, const char*> ShaderLoader::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        auto id = GetAssetId(inFilepath);
        return { id, ShaderAsset::GetStaticAssetClassName() };
    }

    UUID ShaderLoader::GetAssetId(const std::filesystem::path &inFilepath) const
    {
        auto metaFilepath = (inFilepath.parent_path())/inFilepath.filename().replace_extension(".zmeta");
        UUID id;
        if (!std::filesystem::exists(metaFilepath))
        {
            std::ofstream ofs(metaFilepath);
            cereal::JSONOutputArchive archive(ofs);
            archive(cereal::make_nvp("id", (uint64_t)id));
        }
        else
        {
            uint64_t idInt;
            std::ifstream ifs(metaFilepath);
            cereal::JSONInputArchive archive(ifs);
            archive(cereal::make_nvp("id", idInt));
            id = idInt;
        }

        return id;
    }
}