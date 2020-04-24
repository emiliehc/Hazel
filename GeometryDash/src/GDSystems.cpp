#include "GDSystems.h"

#include <glm/glm.hpp>

#include "GDComponents.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/ECS/ECSCore.h"

namespace GD
{
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

    // --------------------------------------------
    // Camera System
    // --------------------------------------------
    CameraSystem::CameraSystem(ECS* ecs) : System(ecs), m_Player(4294967295), m_AspectRatio(1280.0f / 720.0f), m_Camera(
                                               -m_AspectRatio * m_ZoomLevel,
                                               m_AspectRatio * m_ZoomLevel,
                                               -m_ZoomLevel, m_ZoomLevel),
                                           m_Rotation(true)
    {
    }

    void CameraSystem::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(CameraSystem::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(CameraSystem::OnWindowResized));
    }

    bool CameraSystem::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.00001f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                               m_AspectRatio * m_ZoomLevel,
                               -m_ZoomLevel, m_ZoomLevel);

        return false;
    }

    bool CameraSystem::OnWindowResized(WindowResizeEvent& e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                               m_AspectRatio * m_ZoomLevel,
                               -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    void CameraSystem::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(HZ_KEY_S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(HZ_KEY_W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(HZ_KEY_Q))
            {
                m_CameraRotation += m_CameraRotationSpeed * ts;
            }
            if (Input::IsKeyPressed(HZ_KEY_E))
            {
                m_CameraRotation -= m_CameraRotationSpeed * ts;
            }

            if (m_CameraRotation > 180.0f)
            {
                m_CameraRotation -= 360.0f;
            }
            else if (m_CameraRotation <= -180.0f)
            {
                m_CameraRotation += 360.0f;
            }

            m_Camera.SetRotation(m_CameraRotation);
        }

        // update camera position according to the player position
        //m_CameraPosition = m_PlayerTransform->Position;

        m_Camera.SetPosition(m_CameraPosition);

        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void CameraSystem::OnEntityAdded(Entity e)
    {
    }

    void CameraSystem::OnEntityRemoved(Entity e)
    {
    }

    void CameraSystem::SetPlayer(Entity e)
    {
        m_Player = e;
        m_PlayerTransform = &m_ECS->GetComponent<Transform>(e);
    }


    // --------------------------------------------
    // Game Logic System
    // --------------------------------------------
    GameLogicSystem::GameLogicSystem(ECS* ecs) : System(ecs), m_Player(4294967295)
    {
    }

    void GameLogicSystem::OnUpdate(Timestep ts)
    {
        auto& playerTransform = m_ECS->GetComponent<Transform>(m_Player);
        auto& playerRigidBody = m_ECS->GetComponent<RigidBody>(m_Player);
        auto& playerGravity = m_ECS->GetComponent<Gravity>(m_Player);
        auto& playerProps = m_ECS->GetComponent<GDPlayer>(m_Player);
        // physics logic
        // treat the player as a circle for collision detections
        //
        // link normal jumping to Input::IsKeyPressed (input system),
        // but hook mid-air special jumping up to KeyPressedEvent (event system)
        //
        // for reverse portals, just do it

        // first respond to input, e.g. space bar pressed while sliding for jumping
        // depends on the current game mode
        switch (playerProps.GameMode)
        {
        case GDGameMode::Cube:
        {
            if (Input::IsKeyPressed(HZ_KEY_SPACE) && playerProps.OnTheGround)
            {
                playerRigidBody.Velocity.y = 30.0f;
            }
            // for debug
            if (Input::IsKeyPressed(HZ_KEY_RIGHT))
            {
                playerRigidBody.Velocity.x = 5.0f;
            }
            else if (Input::IsKeyPressed(HZ_KEY_LEFT))
            {
                playerRigidBody.Velocity.x = -5.0f;
            }
            else
            {
                playerRigidBody.Velocity.x = 0.0f;
            }

            break;
        }
        default:
        {
            HZ_ASSERT(false, "Game mode not implemented!");
        }
        }

        // updating player props
        // might want to cache the address of its components in the future if deemed necessary for optimization

        playerRigidBody.Acceleration = playerGravity.Force / playerRigidBody.Mass;
        playerRigidBody.Velocity += playerRigidBody.Acceleration * ts.GetSeconds();
        // vertical velocity cap
        playerRigidBody.Velocity.y = std::max(playerRigidBody.Velocity.y, -20.0f);
        playerRigidBody.Velocity.y = std::min(playerRigidBody.Velocity.y, 30.0f);
        // TODO : these limit literals will be extracted later into private fields for better access
        playerTransform.Position += playerRigidBody.Velocity * ts.GetSeconds();


        // always do game logic update before this
        playerProps.OnTheGround = false; // default
        // reset its position if a collision is detected

        for (Entity e : m_Entities)
        {
            // TODO : support rotation
            // TODO : support trangular slope
            auto& objectTransform = m_ECS->GetComponent<Transform>(e);
            // check if it is in range, if not, don't bother with it
            if (playerTransform.Position.x + playerTransform.Size.x / 2 < objectTransform.Position.x - objectTransform
                                                                                                       .Size.x / 2 ||
                playerTransform.Position.x - playerTransform.Size.x / 2 > objectTransform.Position.x + objectTransform
                                                                                                       .Size.x / 2)
            {
                continue;
            }

            auto& objectProps = m_ECS->GetComponent<GDObject>(e);
            // TODO : change to switch expression
            if (objectProps.ObjectType == GDObjectType::Square)
            {
                // top
                float playerBottomToObjectTop = (playerTransform.Position.y - playerTransform.Size.y / 2) - (
                    objectTransform.Position.y + objectTransform.Size.y / 2);
                if (playerBottomToObjectTop <= playerTransform.Size.y / 10.0f && playerBottomToObjectTop >= -playerTransform.Size.y / 2.0f)
                {
                    playerTransform.Position.y = objectTransform.Position.y + objectTransform.Size.y / 2 +
                        playerTransform.Size.y / 2;
                    playerProps.OnTheGround = true;
                    playerRigidBody.Velocity.y = 0.0f;
                    continue;
                }

                // bottom

                // TODO : side
            }
        }
    }

    void GameLogicSystem::OnEvent(Event& event)
    {
        // once a space bar event is received, it sends a signal to the on update functino via some kind of shared variable
    }

    void GameLogicSystem::OnEntityAdded(Entity e)
    {
        if (e == m_Player)
        {
            m_Entities.erase(e);
        }
    }

    void GameLogicSystem::OnEntityRemoved(Entity e)
    {
    }
}
