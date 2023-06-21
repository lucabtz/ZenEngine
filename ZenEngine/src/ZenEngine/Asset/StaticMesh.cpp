#include "StaticMesh.h"

#include "ZenEngine/Renderer/VertexBuffer.h"
#include "ZenEngine/Renderer/IndexBuffer.h"

#include "OBJ_Loader.h"

namespace ZenEngine
{

    std::shared_ptr<VertexArray> StaticMesh::CreateOrGetVertexArray()
    {
        if (mVertexArray != nullptr) return mVertexArray;

        BufferLayout layout{
            { ShaderDataType::Float3, "Position" },
            { ShaderDataType::Float2, "TexCoord" },
            { ShaderDataType::Float3, "Normal" }
        };
        auto vb = VertexBuffer::Create((float*)mVertices.data(), mVertices.size() * sizeof(Vertex));
        vb->SetLayout(layout);

        auto ib = IndexBuffer::Create(mIndices.data(), mIndices.size());

        mVertexArray = VertexArray::Create();
        mVertexArray->AddVertexBuffer(vb);
        mVertexArray->SetIndexBuffer(ib);
        return mVertexArray;
    }

    std::vector<std::shared_ptr<AssetType>> OBJImporter::Import(const std::filesystem::path &inFilepath)
    {
        objl::Loader Loader;
        bool loadout = Loader.LoadFile(inFilepath.string());
        std::vector<std::shared_ptr<AssetType>> meshes;
        if (!loadout)
        {
            ZE_CORE_ERROR("Could not open {} as OBJ file", inFilepath);
            return meshes;
        }
        
        for (int i = 0; i < Loader.LoadedMeshes.size(); ++i)
        {
            objl::Mesh curMesh = Loader.LoadedMeshes[i];
            std::shared_ptr<StaticMesh> mesh = std::make_unique<StaticMesh>();

            for (auto &vertex : curMesh.Vertices)
            {
                mesh->GetVertices().push_back({
                    { vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
                    { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y },
                    { vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z }
                });
            }

            mesh->GetIndices() = curMesh.Indices;
            meshes.push_back(mesh);
            /*file << "Material: " << curMesh.MeshMaterial.name << "\n";
            file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
            file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
            file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
            file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
            file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
            file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
            file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
            file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
            file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
            file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
            file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
            file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";*/
        }
        return meshes;
    }

}