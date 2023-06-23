#include "AssetManagerDatabase.h"

#include "EditorGUI.h"
#include "ZenEngine/Asset/AssetManager.h"

namespace ZenEngine
{
    void AssetManagerDatabase::OnRenderWindow()
    {
        if (ImGui::Button("Refresh asset database"))
        {
            AssetManager::Get().BuildAssetDatabase();
        }

        for (auto &[id, asset]: AssetManager::Get().GetAssetDatabase())
        {
            std::string idString = fmt::format("{}", (uint64_t)asset.Id);
            ImGui::PushID(idString.c_str());
            EditorGUI::SelectableText("Id", idString);
            EditorGUI::SelectableText("Class", asset.Class->Name);
            EditorGUI::SelectableText("Path", asset.Filepath.string());
            ImGui::Button("Drag me");
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload(asset.Class->Name.c_str(), (uint64_t*)&id, sizeof(uint64_t));
                ImGui::EndDragDropSource();
            }
            ImGui::Separator();
            ImGui::PopID();
        }
    }
}