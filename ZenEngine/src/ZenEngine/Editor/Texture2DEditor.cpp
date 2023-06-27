#include "Texture2DEditor.h"

#include "EditorGUI.h"

namespace ZenEngine
{

    void Texture2DEditor::OnRenderWindow()
    {
        auto id = mAssetInstance->CreateOrGetTexture2D()->GetRendererID();
        auto props = mAssetInstance->GetTextureProperties();
        ImGui::Image(reinterpret_cast<void*>(id), { (float)props.Width, (float)props.Height });

        EditorGUI::SelectableText("Width", fmt::format("{}", props.Width).c_str());
        EditorGUI::SelectableText("Height", fmt::format("{}", props.Height).c_str());
        switch (props.Format)
        {
        case Texture2D::Format::R8: EditorGUI::SelectableText("Format", "R8"); break;
        case Texture2D::Format::RGB8: EditorGUI::SelectableText("Format", "RGB"); break;
        case Texture2D::Format::RGBA8: EditorGUI::SelectableText("Format", "RGBA8"); break;
        case Texture2D::Format::RGBA32F: EditorGUI::SelectableText("Format", "RGBA32F"); break;
        default: EditorGUI::SelectableText("Format", "Unknown");
        }
    }
}