#pragma once

#include "AssetInstance.h"
#include "ZenEngine/Renderer/Shader.h"
#include "ZenEngine/ShaderCompiler/ShaderReflector.h"
#include "ZenEngine/Renderer/RendererAPI.h"

namespace ZenEngine
{
    class ShaderAsset : public AssetInstance
    {
    public:
        IMPLEMENT_ASSET_CLASS(ZenEngine::ShaderAsset)
        ShaderAsset() = default;

        std::shared_ptr<Shader> CreateOrGetShaderProgram();
        Shader::ShaderUniformInfo GetShaderUniforms();
    
        void SetName(const std::string &inName) { mName = inName; }
        void SetSourceCode(const std::string &inSource) { mSourceCode = inSource; mTainted = true; }

        const std::string &GetName() const { return mName; }
        const std::string &GetSourceCode() const { return mSourceCode; }
    private:
        std::string mName;
        std::string mSourceCode;

        bool mTainted = false;
        std::shared_ptr<Shader> mShaderProgram;        
    };

    class ShaderSerializer : public AssetSerializer
    {
    public:
        IMPLEMENT_SERIALIZER_CLASS(ZenEngine::ShaderSerializer)

        virtual bool Save(const std::shared_ptr<AssetInstance> &inAssetInstance, const Asset &inAsset) const override;
        virtual std::shared_ptr<AssetInstance> Load(const Asset &inAsset) const override;
        virtual bool CanSerialize(const std::filesystem::path &inFilepath) const override;
        virtual std::pair<UUID, const AssetClass*> GetAssetIdAssetClass(const std::filesystem::path &inFilepath) const override;
    };
}

CEREAL_REGISTER_TYPE(ZenEngine::ShaderAsset);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ZenEngine::AssetInstance, ZenEngine::ShaderAsset)