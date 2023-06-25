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
    
    bool ShaderSerializer::Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const
    {
        ZE_ASSERT_CORE_MSG(false, "ShaderSerializer::Save is not supposed to be called since there are no shader importers! Shaders use a text format an can just be copied in the assets directory.");
        return false;
    }
    
    std::shared_ptr<AssetInstance> ShaderSerializer::Load(const Asset &inAsset) const
    {
        std::shared_ptr<ShaderAsset> shader = std::make_unique<ShaderAsset>();
        shader->SetSourceCode(Filesystem::ReadFileToString(inAsset.Filepath));
        auto asset = std::static_pointer_cast<AssetInstance>(shader);
        SetId(asset, GetAssetId(inAsset.Filepath));
        return asset;
    }

    bool ShaderSerializer::CanSerialize(const std::filesystem::path &inFilepath) const
    {
        return (inFilepath.extension().string() == ".zshader" || inFilepath.extension().string() == ".hlsl");
    }

    std::pair<UUID, const char*> ShaderSerializer::GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const
    {
        auto id = GetAssetId(inFilepath);
        return { id, ShaderAsset::GetStaticAssetClassName() };
    }

    UUID ShaderSerializer::GetAssetId(const std::filesystem::path &inFilepath) const
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