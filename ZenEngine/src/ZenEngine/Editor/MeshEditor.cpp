#include "MeshEditor.h"

namespace ZenEngine
{

    void MeshEditor::OnRenderWindow()
    {
        EditorGUI::SelectableText("Vertex count", fmt::format("{}", mAssetInstance->GetVertices().size()));
        EditorGUI::SelectableText("Index count", fmt::format("{}", mAssetInstance->GetIndices().size()));
    }
}