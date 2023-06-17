#pragma once

#include <mutex>

#include <imgui.h>
#include <imgui_internal.h>

#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Event/Event.h"

namespace ZenEngine
{
    class EditorGUI
    {
    public:
        EditorGUI();
        void Init();
        void Shutdown();

        void OnEvent(const std::unique_ptr<Event> &inEvent);

        void BeginGUI();
        void EndGUI();

        static EditorGUI &Get() { ZE_ASSERT_CORE_MSG(sEditorGUIInstance != nullptr, "EditorGUI does not exist!"); return  *sEditorGUIInstance; }
    private:
        static EditorGUI *sEditorGUIInstance;
    };
};