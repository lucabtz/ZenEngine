#pragma once

#include <glm/glm.hpp>
#include "AssetInstance.h"

#include "Serialization.h"
#include "ZenEngine/Renderer/VertexArray.h"

namespace ZenEngine
{
    struct alignas(float) Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;

        template<typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(Position, Normal, TexCoord); 
        }
    };
    static_assert(sizeof(Vertex) == 8 * sizeof(float));

    class StaticMesh : public AssetInstance
    {
    public:
        IMPLEMENT_ASSET_CLASS(ZenEngine::StaticMesh)

        void SetVertices(const std::vector<Vertex> &inVertices) { mVertices = inVertices; mTainted = true; }
        void SetIndices(const std::vector<uint32_t> &inIndices) { mIndices = inIndices; mTainted = true; }
        const std::vector<Vertex> &GetVertices() { return mVertices; }
        const std::vector<uint32_t> &GetIndices() { return mIndices; }
        
        void PushVertex(Vertex inVertex) { mVertices.push_back(inVertex); mTainted = true; }
        void PushTriangle(uint32_t inIndices[3]) { for (int i = 0; i < 3; ++i) PushIndex(inIndices[i]); mTainted = true; }
        void PushIndex(uint32_t inIndex) {  mIndices.push_back(inIndex); mTainted = true; }

        std::shared_ptr<VertexArray> CreateOrGetVertexArray();
    private:
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        bool mTainted = false;

        std::shared_ptr<VertexArray> mVertexArray;

        template<typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(mVertices, mIndices); 
        }
        
        friend class cereal::access;
    };

    class OBJImporter : public AssetImporter
    {
    public:
        virtual const char *GetName() const { return "OBJImporter"; }
        virtual std::unordered_set<std::string> GetImportableExtensions() const 
        {
            return { ".obj" };
        }

        virtual std::vector<ImportedAsset> Import(const std::filesystem::path &inFilepath) override;
    };
}

CEREAL_REGISTER_TYPE(ZenEngine::StaticMesh);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ZenEngine::AssetInstance, ZenEngine::StaticMesh)