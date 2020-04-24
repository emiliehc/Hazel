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
        void OnUpdate(Hazel::Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Hazel::Event& event) override;

    private:
        OrthographicCamera* m_Camera;
    };
}
