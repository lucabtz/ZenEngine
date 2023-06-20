#include "AssetManagerInfo.h"

#include "ZenEngine/Asset/AssetManager.h"

namespace ZenEngine
{
    void AssetManagerInfo::OnRenderWindow()
    {
        for (auto &[id, asset]: AssetManager::Get().GetAssetDatabase())
        {
            ImGui::Separator();
            ImGui::Text("Id: %llu", (uint64_t)asset.Id);
            ImGui::Text("%s: %s", asset.Class.Name.c_str(), asset.Filepath.string().c_str());
            ImGui::Text("Loaded: %s", AssetManager::Get().IsLoaded(asset.Id)? "true" : "false");
            ImGui::Separator();
        }
    }
}