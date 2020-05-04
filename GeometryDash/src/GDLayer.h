#pragma once
#include "Hazel/Core/Layer.h"
#include "Hazel/Renderer/OrthographicCameraController.h"


using namespace Hazel;

namespace GD
{
    class GDLayer : public Layer
    {
    public:
        GDLayer();
        ~GDLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;

    private:
        std::vector<ECS> m_ECSBackups;
        Entity m_Player;
    };
}
