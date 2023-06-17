#pragma once

#include <string>
#include <memory>

#include "ZenEngine/Event/Event.h"

namespace ZenEngine
{
    class Layer
    {
    public:
		Layer(const std::string& inName = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float inDeltaTime) {}
		virtual void OnRender(float inDeltaTime) {}
		virtual void OnEvent(const std::unique_ptr<Event> &inEvent) {}
		virtual void OnRenderEditorGUI() {}

		const std::string &GetName() const { return mName; }
	protected:
		std::string mName;
    };

    class LayerStack
	{
	public:
		LayerStack() = default;
        ~LayerStack();

		void PushLayer(std::unique_ptr<Layer> inLayer);
        void RemoveLayer(const std::unique_ptr<Layer> &inLayer) { RemoveLayer(inLayer.get()); }
        void RemoveLayer(Layer *inLayer);

		std::vector<std::unique_ptr<Layer>>::iterator begin() { return mLayers.begin(); }
		std::vector<std::unique_ptr<Layer>>::iterator end() { return mLayers.end(); }
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() { return mLayers.rbegin(); }
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() { return mLayers.rend(); }

		std::vector<std::unique_ptr<Layer>>::const_iterator begin() const { return mLayers.begin(); }
		std::vector<std::unique_ptr<Layer>>::const_iterator end()	const { return mLayers.end(); }
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const { return mLayers.rend(); }
	private:
		std::vector<std::unique_ptr<Layer>> mLayers;
	};
}