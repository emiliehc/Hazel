#include "GDSystems.h"

#include <glm/glm.hpp>

#include "GDComponents.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/ECS/ECSCore.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace GD
{
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

    // --------------------------------------------
    // Custom Renderer System
    // --------------------------------------------
    GDRendererSystem::GDRendererSystem(ECS* ecs) : System(ecs)
    {
    }

    Ref<System> GDRendererSystem::Clone() const
    {
        return std::static_pointer_cast<System>(std::make_shared<GDRendererSystem>(*this));
    }

    void GDRendererSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities)
        {
            // render
            // quads
            const auto& transform = m_ECS->GetComponent<Transform>(e);
            const auto& color = m_ECS->HasComponent<Colored>(e) ? m_ECS->GetComponent<Colored>(e).Color : m_AccentColor;
            
            auto type = m_ECS->GetComponent<Drawable>(e).GeometryType;
            if (type == PrimitiveGeometryType::Quad)
            {
                // has rotation?
                if (transform.Rotation == 0.0f)
                {
                    // has texture?
                    if (m_ECS->HasComponent<Textured>(e))
                    {
                        const auto& textureData = m_ECS->GetComponent<Textured>(e);
                        Renderer2D::DrawQuad(transform.Position, transform.Size, textureData.Texture,
                                             textureData.TilingFactor, color);
                    }
                    else
                    {
                        Renderer2D::DrawQuad(transform.Position, transform.Size, color);
                    }
                }
                else
                {
                    // has texture?
                    if (m_ECS->HasComponent<Textured>(e))
                    {
                        const auto& textureData = m_ECS->GetComponent<Textured>(e);
                        Renderer2D::DrawRotatedQuad(transform.Position, transform.Size, transform.Rotation,
                                                    textureData.Texture, textureData.TilingFactor, color);
                    }
                    else
                    {
                        Renderer2D::DrawRotatedQuad(transform.Position, transform.Size, transform.Rotation, color);
                    }
                }
            }
        }
    }

    // --------------------------------------------
    // Camera System
    // --------------------------------------------
    GDCameraSystem::GDCameraSystem(ECS* ecs) : System(ecs), m_Player(4294967295), m_AspectRatio(1280.0f / 720.0f),
                                               m_Camera(
                                                   -m_AspectRatio * m_ZoomLevel,
                                                   m_AspectRatio * m_ZoomLevel,
                                                   -m_ZoomLevel, m_ZoomLevel),
                                               m_Rotation(true)
    {
    }

    void GDCameraSystem::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(GDCameraSystem::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(GDCameraSystem::OnWindowResized));
    }

    bool GDCameraSystem::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.00001f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                               m_AspectRatio * m_ZoomLevel,
                               -m_ZoomLevel, m_ZoomLevel);

        return false;
    }

    bool GDCameraSystem::OnWindowResized(WindowResizeEvent& e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                               m_AspectRatio * m_ZoomLevel,
                               -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    void GDCameraSystem::OnUpdate(Timestep ts)
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

    void GDCameraSystem::OnEntityAdded(Entity e)
    {
    }

    void GDCameraSystem::OnEntityRemoved(Entity e)
    {
    }

    void GDCameraSystem::SetPlayer(Entity e)
    {
        m_Player = e;
        m_PlayerTransform = &m_ECS->GetComponent<Transform>(e);
    }


    // --------------------------------------------
    // Game Logic System
    // --------------------------------------------
    GDGameLogicSystem::GDGameLogicSystem(ECS* ecs) : System(ecs), m_Player(4294967295)
    {
    }

    void GDGameLogicSystem::OnUpdate(Timestep ts)
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
        playerRigidBody.Velocity.y = std::min(playerRigidBody.Velocity.y, 25.0f);
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
            switch (objectProps.ObjectType)
            {
            case GDObjectType::Square:
            {
                // top
                float playerBottomToObjectTop = (playerTransform.Position.y - playerTransform.Size.y / 2) - (
                    objectTransform.Position.y + objectTransform.Size.y / 2);
                if (playerBottomToObjectTop <= playerTransform.Size.y / 10.0f && playerBottomToObjectTop >= -
                    playerTransform.Size.y / 2.0f)
                {
                    playerTransform.Position.y = objectTransform.Position.y + objectTransform.Size.y / 2 +
                        playerTransform.Size.y / 2;
                    playerProps.OnTheGround = true;
                    playerRigidBody.Velocity.y = 0.0f;
                    break;
                }

                // bottom
                float objectBottomToPlayerTop; // TODO

                // TODO : side
                break;
            }
            case GDObjectType::Ground:
            {
                // only check top
                float playerBottomToObjectTop = (playerTransform.Position.y - playerTransform.Size.y / 2) - (
                    objectTransform.Position.y + objectTransform.Size.y / 2);
                if (playerBottomToObjectTop <= playerTransform.Size.y / 10.0f && playerBottomToObjectTop >= -
                    playerTransform.Size.y / 2.0f)
                {
                    playerTransform.Position.y = objectTransform.Position.y + objectTransform.Size.y / 2 +
                        playerTransform.Size.y / 2;
                    playerProps.OnTheGround = true;
                    playerRigidBody.Velocity.y = 0.0f;
                }
                break;
            }
            default:
            {
                HZ_ASSERT(false, "Unsupported object type!");
            }
            }
        }
    }

    void GDGameLogicSystem::OnEvent(Event& event)
    {
        // once a space bar event is received, it sends a signal to the on update functino via some kind of shared variable (for single threaded design only)
        // get key repeat count. if the repeat count is 1 or greater, ignore it, because that will be managed in the OnUpdate function
    }

    void GDGameLogicSystem::OnEntityAdded(Entity e)
    {
        if (e == m_Player)
        {
            m_Entities.erase(e);
        }
    }

    void GDGameLogicSystem::OnEntityRemoved(Entity e)
    {
    }

    void GDGameLogicSystem::SetAccentColor(const glm::vec4& accentColor)
    {
        m_AccentColor = accentColor;
        m_ECS->GetSystem<GDRendererSystem>()->SetAccentColor(accentColor);
    }
}
