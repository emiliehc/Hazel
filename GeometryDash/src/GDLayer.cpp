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
        // deregister custom systems
        m_ECS.DeregisterSystem<RendererSystem>();

        // register additional components
        m_ECS.RegisterComponent<GDPlayer>();
        m_ECS.RegisterComponent<GDObject>();

        // register additional systems
        auto rendererSys = m_ECS.RegisterSystem<GDRendererSystem>();
        {
            Signature signature;
            signature.set(m_ECS.GetComponentType<Transform>());
            signature.set(m_ECS.GetComponentType<Drawable>());
            m_ECS.SetSystemSignature<GDRendererSystem>(signature);
        }
        auto gameLogicSys = m_ECS.RegisterSystem<GDGameLogicSystem>();
        {
            Signature signature;
            signature.set(m_ECS.GetComponentType<Transform>());
            signature.set(m_ECS.GetComponentType<GDObject>());
            m_ECS.SetSystemSignature<GDGameLogicSystem>(signature);
        }
        auto camSys = m_ECS.RegisterSystem<GDCameraSystem>();

        // create entities
        // create player
        Entity player = m_ECS.CreateEntity();
        m_ECS.AddComponent<Transform>(player, {{0.0f, 100.0f, 0.9f}, {1.0f, 1.0f}, 0.0f});
        //m_ECS.AddComponent<Colored>(player, {glm::vec4(1.0f)});
        m_ECS.AddComponent<Drawable>(player, {PrimitiveGeometryType::Quad});
        m_ECS.AddComponent<GDPlayer>(player, {GDGameMode::Cube, false});
        m_ECS.AddComponent<Gravity>(player, {{0.0f, -140.0f, 0.0f}});
        m_ECS.AddComponent<RigidBody>(player, {glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, true});
        m_ECS.AddComponent<GDObject>(player, {GDObjectType::Player});
        gameLogicSys->SetPlayer(player);
        camSys->SetPlayer(player);

        // ground
        for (int i = 0; i < 10; i++)
        {
            Entity ground = m_ECS.CreateQuad({0.0f + i * 3.0f, 0.0f, 0.0f}, {3.0f, 4.0f}, {0.3f, 0.8f, 0.2f, 1.0f});
            m_ECS.AddComponent<GDObject>(ground, {GDObjectType::Ground});
        }

        auto textureSquare = Texture2D::Create("assets/res/groundSquare_01_001-uhd.png");
        for (int i = 0; i < 3; i++)
        {
            Entity square = m_ECS.CreateQuad({7.0f + i * 2.0f, 3.0f + i * 0.5f, 0.0f}, {1.0f, 1.0f}, textureSquare);
            m_ECS.RemoveComponent<Colored>(square);
            m_ECS.AddComponent<GDObject>(square, {GDObjectType::Square});
        }


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

        Renderer2D::BeginScene(m_ECS.GetSystem<GDCameraSystem>()->GetCamera());

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
