#include "CoreSystems.h"
#include "Scene.h"

#include "CoreComponents.h"
#include "ZenEngine/Renderer/Renderer.h"

namespace ZenEngine
{
    void StaticMeshRendererSystem::OnRender(float inDeltaTime)
    {
        auto view = mScene->View<TransformComponent, StaticMeshComponent>();
        for (auto entt : view)
        {
            Entity entity(entt, mScene);
            auto &smc = view.get<StaticMeshComponent>(entt);
            auto &tc = view.get<TransformComponent>(entt);
            if (smc.MeshVertexArray == nullptr || smc.Mat == nullptr) continue;
            Renderer::Get().Submit(smc.MeshVertexArray, entity.GetWorldTransform(), smc.Mat);
        }
    }
}