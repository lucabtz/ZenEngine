#pragma once

#include <glm/glm.hpp>
#include "AssetType.h"

#include "Serialization.h"
#include "ZenEngine/Renderer/VertexArray.h"

namespace ZenEngine
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec3 Normal;

        template<typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(Position, TexCoord, Normal); 
        }
    };

    class StaticMesh : public AssetType
    {
    public:
        IMPLEMENT_ASSET_CLASS(ZenEngine::StaticMesh)

        std::vector<Vertex> &GetVertices() { return mVertices; }
        std::vector<uint32_t> &GetIndices() { return mIndices; }

        std::shared_ptr<VertexArray> CreateOrGetVertexArray();
    private:
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;

        std::shared_ptr<VertexArray> mVertexArray;

        template<typename Archive>
        void Serialize(Archive &inArchive)
        {
            inArchive(mVertices, mIndices); 
        }
        
        friend class cereal::access;
    };

    class OBJImporter : public AssetImporterFor<StaticMesh>
    {
    public:
        virtual const char *GetName() const { return "OBJImporter"; }
        virtual std::vector<std::string> ProvidesForExtensions() const 
        {
            return { ".obj" };
        }

        virtual std::vector<std::shared_ptr<AssetType>> Import(const std::filesystem::path &inFilepath) override;
    };
}

CEREAL_REGISTER_TYPE(ZenEngine::StaticMesh);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ZenEngine::AssetType, ZenEngine::StaticMesh)