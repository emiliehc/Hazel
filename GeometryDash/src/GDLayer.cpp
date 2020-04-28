#include "GDLayer.h"


#include "GDAssetManager.h"
#include "GDComponents.h"
#include "GDObjectCreator.h"
#include "GDStaticParticleSystem.h"
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
        // load assets
        GDAssetManager::Init();

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
        Entity player = CreatePlayer(m_ECS);
        gameLogicSys->SetPlayer(player);
        camSys->SetPlayer(player);

        // ground
        for (int i = 0; i < 20; i++)
        {
            CreateGround(m_ECS, {0.0f + i * 5.0f, -3.0f, 0.0f});
        }

        // squares
        for (int i = 0; i < 3; i++)
        {
            CreateSquare(m_ECS, {7.0f + i * 2.0f, 3.0f + i * 0.5f, 0.0f});
        }

        // spikes
        for (int i = 0; i < 3; i++)
        {
            CreateTriangle(m_ECS, {20.0f + i, 2.5f, 0.0f});
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
        RenderCommand::SetClearColor({0.3f, 0.3f, 0.3f, 1.0f});
        RenderCommand::Clear();

        Renderer2D::BeginScene(m_ECS.GetSystem<GDCameraSystem>()->GetCamera());

        Layer::OnUpdate(ts);
        GDParticleSystem::OnUpdate(ts);
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
