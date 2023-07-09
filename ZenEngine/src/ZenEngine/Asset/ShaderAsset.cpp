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
    
    bool ShaderLoader::Save(const std::shared_ptr<Asset> &inAssetInstance, const std::filesystem::path &inFilepath) const
    {
        inAssetInstance->GetAssetId();
        std::ofstream ofs(inFilepath);
        ofs << std::static_pointer_cast<ShaderAsset>(inAssetInstance)->GetSourceCode();
        SaveMeta(inAssetInstance, {}, inFilepath);
        return true;
    }
    
    std::shared_ptr<Asset> ShaderLoader::Load(const std::filesystem::path &inFilepath) const
    {
        std::shared_ptr<ShaderAsset> shader = std::make_unique<ShaderAsset>();
        shader->SetName(inFilepath.stem().string());
        shader->SetSourceCode(Filesystem::ReadFileToString(inFilepath));
        auto asset = std::static_pointer_cast<Asset>(shader);
        auto meta = LoadMeta(inFilepath);
        SetId(asset, meta.AssetMeta.Id);
        return asset;
    }

    bool ShaderLoader::CanLoad(const std::filesystem::path &inFilepath) const
    {
        return (inFilepath.extension().string() == ".zshader" || inFilepath.extension().string() == ".hlsl");
    }
}