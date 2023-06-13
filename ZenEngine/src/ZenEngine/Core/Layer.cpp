#include "Layer.h"

#include "Log.h"

namespace ZenEngine
{
    Layer::Layer(const std::string &inName)
        : mName(inName)
    {}

    LayerStack::~LayerStack()
    {
        for (auto & layer : mLayers)
        {
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(std::unique_ptr<Layer> inLayer)
    {
        ZE_CORE_TRACE("Pushing layer {}", inLayer->GetName());
        inLayer->OnAttach();
        mLayers.push_back(std::move(inLayer));
    }

    void LayerStack::RemoveLayer(Layer *inLayer)
    {
        auto it = std::find_if(mLayers.begin(), mLayers.end(), [inLayer](auto &layer) { return inLayer == layer.get(); });
        if (it != mLayers.end())
        {
            ZE_CORE_TRACE("Detaching layer {}", inLayer->GetName());
            inLayer->OnDetach();
            mLayers.erase(it);
        }
    }
}
