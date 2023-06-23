#include "CoreSystems.h"
#include "Scene.h"

#include "CoreComponents.h"
#include "ZenEngine/Renderer/Renderer.h"

namespace ZenEngine
{
    void StaticMeshRendererSystem::OnRender(float inDeltaTime)
    {
        auto view = mScene->View<TransformComponent, StaticMeshComponent>();
        for (auto entity : view)
        {
            auto &smc = view.get<StaticMeshComponent>(entity);
            auto &tc = view.get<TransformComponent>(entity);
            if (smc.Mesh == nullptr || smc.Mat == nullptr) continue;
            Renderer::Get().Submit(smc.Mesh->CreateOrGetVertexArray(), tc.GetTransform(), smc.Mat);
        }
    }
}