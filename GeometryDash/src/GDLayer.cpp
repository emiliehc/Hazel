#include "GDLayer.h"


#include "GDComponents.h"
#include "GDSystems.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Renderer/RenderCommand.h"

using namespace Hazel;

namespace GD
{
    GDLayer::GDLayer() : Layer("MainGame")
    {
        // register additional components
        m_ECS.RegisterComponent<GDPlayer>();
        m_ECS.RegisterComponent<GDObject>();

        // register additional systems
        auto gameLogicSys = m_ECS.RegisterSystem<GameLogicSystem>();
        {
            Signature signature;
            signature.set(m_ECS.GetComponentType<Transform>());
            signature.set(m_ECS.GetComponentType<GDObject>());
            m_ECS.SetSystemSignature<GameLogicSystem>(signature);
        }
        auto camSys = m_ECS.RegisterSystem<CameraSystem>();

        // create entities
        // create player
        Entity player = m_ECS.CreateEntity();
        m_ECS.AddComponent<Transform>(player, {{0.0f, 100.0f, 0.9f}, {1.0f, 1.0f}, 0.0f});
        m_ECS.AddComponent<Colored>(player, {glm::vec4(1.0f)});
        m_ECS.AddComponent<Drawable>(player, {PrimitiveGeometryType::Quad});
        m_ECS.AddComponent<GDPlayer>(player, {GDGameMode::Cube, false});
        m_ECS.AddComponent<Gravity>(player, {{0.0f, -140.0f, 0.0f}});
        m_ECS.AddComponent<RigidBody>(player, {glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, true});
        m_ECS.AddComponent<GDObject>(player, {GDObjectType::Player});
        gameLogicSys->SetPlayer(player);
        camSys->SetPlayer(player);

        Entity quad = m_ECS.CreateQuad({0.0f, 0.0f, 0.0f}, {3.0f, 4.0f}, {0.3f, 0.8f, 0.2f, 1.0f});
        m_ECS.AddComponent<GDObject>(quad, {GDObjectType::Square});

        // init camera
        m_Camera = &camSys->GetCamera();
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

        Renderer2D::BeginScene(m_ECS.GetSystem<CameraSystem>()->GetCamera());

        Layer::OnUpdate(ts);

        Renderer2D::EndScene();
    }

    void GDLayer::OnImGuiRender()
    {
    }

    void GDLayer::OnEvent(Event& event)
    {
        Layer::OnEvent(event);
        static ECS ecs;
        // snapshot
        if (event.GetEventType() == KeyPressedEvent::GetStaticType())
        {
            KeyPressedEvent* e = (KeyPressedEvent*)&event;
            if (e->GetKeyCode() == HZ_KEY_X)
            {
                ecs = m_ECS;
            }
            if (e->GetKeyCode() == HZ_KEY_Z)
            {
                m_ECS = ecs;
            }
        }
    }
}
