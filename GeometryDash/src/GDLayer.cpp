#include "GDLayer.h"


#include "GDAssetManager.h"
#include "GDComponents.h"
#include "GDObjectCreator.h"
#include "GDStaticParticleSystem.h"
#include "GDSystems.h"
#include "HazelAudio.h"
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
        // create background
        for (int i = -2; i < 5; i++)
        {
            CreateBackground(m_ECS, {0.0f + 20.0f * i, 9.0f, -0.9f});
        }

        // ground
        for (int i = -10; i < 20; i++)
        {
            CreateGround(m_ECS, {0.0f + i * 5.0f, -5.0f, 0.8f});
        }

        // squares
        for (int i = 0; i < 500; i += 2)
        {
            CreateSquare(m_ECS, {7.0f + i * 2.5f, 1.0f + i * 0.5f, 0.0f});
        }

        // spikes
        for (int i = 0; i < 3; i++)
        {
            CreateTriangle(m_ECS, {20.0f + i, 0.5f, 0.0f});
        }

        // create player
        Entity player = CreatePlayer(m_ECS);
        gameLogicSys->SetPlayer(player);
        camSys->SetPlayer(player);
        m_Player = player;

        // start audio
        auto* stayInsideMe = GDAssetManager::GetAudioSource("StayInsideMe.mp3");
        stayInsideMe->SetLoop(true);
        Audio::Play(*stayInsideMe);

        // ecs backup
        m_ECSBackups.push_back(m_ECS);
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
        // player death check
        const auto& playerProps = m_ECS.GetComponent<GDPlayer>(m_Player);
        static float timeElapsed = 0.0f;
        if (!playerProps.Alive)
        {
            if (timeElapsed == 0.0f)
            {
                // first time, play death audio
                Audio::Play(*GDAssetManager::GetAudioSource("explode_11.ogg"));
            }
            // accumulate time up to a point before resetting the ECS
            timeElapsed += ts;
            if (timeElapsed > 1.0f)
            {
                // return to the last point
                m_ECS = m_ECSBackups.back();
                timeElapsed = 0.0f;
            }
        }

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
        // snapshot
        if (event.GetEventType() == KeyPressedEvent::GetStaticType())
        {
            KeyPressedEvent* e = (KeyPressedEvent*)&event;
            if (e->GetKeyCode() == HZ_KEY_X)
            {
                if (!m_ECSBackups.size() > 1)
                {
                    m_ECSBackups.pop_back();
                }
            }
            if (e->GetKeyCode() == HZ_KEY_Z)
            {
                m_ECSBackups.push_back(m_ECS);
            }
        }
        else if (event.GetEventType() == WindowResizeEvent::GetStaticType())
        {
            // reset ecs backup
            m_ECSBackups.push_back(m_ECS);
        }
    }
}
