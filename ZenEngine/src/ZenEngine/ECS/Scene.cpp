#include "Scene.h"
#include "Entity.h"
#include "System.h"

#include "CoreComponents.h"
#include "CoreSystems.h"

namespace ZenEngine
{
    Scene::Scene()
    {
        RegisterSystem<StaticMeshRendererSystem>();
    }

    Entity Scene::CreateEntity()
    {
        return CreateEntity("Entity");
    }

    Entity Scene::CreateEntity(const std::string &inName)
    {
        Entity e(mRegistry.create(), this);
        e.AddComponent<NameComponent>(inName);
        return e;
    }

    std::vector<Entity> Scene::GetEntitiesByName(const std::string &inName)
    {
        std::vector<Entity> result;
        auto view = mRegistry.view<NameComponent>();
        for (auto entity : view)
        {
            const NameComponent& tc = view.get<NameComponent>(entity);
            if (tc.Name == inName)
                result.push_back(Entity(entity, this));
        }
        return result;
    }

    Entity Scene::GetEntityByName(const std::string &inTag)
    {
        auto entities = GetEntitiesByName(inTag);
        if (entities.size() > 0) return entities[0];
        return Entity::Null;
    }

    void Scene::OnRender(float inDeltaTime)
    {
        for (auto &system : mSystems)
        {
            system->OnRender(inDeltaTime);
        }
    }

    void Scene::OnUpdate(float inDeltaTime)
    {
        for (auto &system : mSystems)
        {
            system->OnUpdate(inDeltaTime);
        }
    }
    Renderer::LightInfo Scene::GetLights()
    {
        Renderer::LightInfo lightInfo{};
        auto directionalLightView = mRegistry.view<DirectionalLightComponent, TransformComponent>();
        auto directionalLightEntity = directionalLightView.front();
        if (directionalLightEntity != entt::null)
        {
            auto &dlc = directionalLightView.get<DirectionalLightComponent>(directionalLightEntity);
            auto &tc = directionalLightView.get<TransformComponent>(directionalLightEntity);
            lightInfo.Directional.DirectionalLightIntensity = dlc.Intensity;
            lightInfo.Directional.DirectionalLightColor = dlc.Color;
            lightInfo.Directional.DirectionalLightDirection = tc.GetForwardVector();
        }
        auto ambientLightView = mRegistry.view<AmbientLightComponent>();
        auto ambientLightEntity = ambientLightView.front();
        if (ambientLightEntity != entt::null)
        {
            auto &alc = ambientLightView.get<AmbientLightComponent>(ambientLightEntity);
            lightInfo.Ambient = alc.Info;
        }
        return lightInfo;
    }
}