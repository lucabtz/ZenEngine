#include "MeshEditor.h"

namespace ZenEngine
{

    void MeshEditor::OnRenderWindow()
    {
        EditorGUI::SelectableText("Vertex count", fmt::format("{}", mAssetInstance->GetVertices().size()));
        EditorGUI::SelectableText("Triangle count", fmt::format("{}", mAssetInstance->GetIndices().size()/3));
    }
}