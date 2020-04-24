#include "GDLayer.h"


#include "GDSystems.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Renderer/RenderCommand.h"

using namespace Hazel;

namespace GD
{
    GDLayer::GDLayer() : Layer("MainGame")
    {
        m_Camera = &m_ECS.RegisterSystem<CameraSystem>()->GetCamera();

        m_ECS.CreateQuad({0.0f, 0.0f, 0.0f}, {3.0f, 4.0f}, {0.3f, 0.8f, 0.2f, 1.0f});
    }

    GDLayer::~GDLayer()
    {
    }

    void GDLayer::OnAttach()
    {
    }

    void GDLayer::OnDetach()
    {
    }

    void GDLayer::OnUpdate(Timestep ts)
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        Renderer2D::BeginScene(*m_Camera);

        Layer::OnUpdate(ts);

        Renderer2D::EndScene();
    }

    void GDLayer::OnImGuiRender()
    {
    }

    void GDLayer::OnEvent(Event& event)
    {
        Layer::OnEvent(event);
    }
}
