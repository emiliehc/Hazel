#include "GDSystems.h"

#include <glm/glm.hpp>

#include "GDComponents.h"
#include "GDStaticParticleSystem.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/ECS/ECSCore.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace GD
{
    // utility functions
    static float sign(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    static bool IsPointInTriangle(const glm::vec2& pt, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3)
    {
        float d1 = sign(pt, v1, v2);
        float d2 = sign(pt, v2, v3);
        float d3 = sign(pt, v3, v1);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }


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
            const auto& objectProps = m_ECS->GetComponent<GDObject>(e);
            if (!objectProps.Visible)
            {
                continue;
            }
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
        ResetBounds();
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
        ResetBounds();
        return false;
    }

    bool GDCameraSystem::OnWindowResized(WindowResizeEvent& e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                               m_AspectRatio * m_ZoomLevel,
                               -m_ZoomLevel, m_ZoomLevel);
        ResetBounds();

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
        const auto& playerPos = m_ECS->GetComponent<Transform>(m_Player).Position;
        const auto& playerVel = m_ECS->GetComponent<RigidBody>(m_Player).Velocity;
        /*
        if (playerPos.x < m_CameraPosition.x + left)
        {
            m_CameraPosition.x = playerPos.x - left;
        }
        if (playerPos.x > m_CameraPosition.x + right)
        {
            m_CameraPosition.x = playerPos.x - right;
        }
        */
        if (playerVel.x > 0)
        {
            m_CameraPosition.x = playerPos.x - left;
        }
        else if (playerVel.x < 0)
        {
            m_CameraPosition.x = playerPos.x - right;
        }
        if (playerPos.y < m_CameraPosition.y + bottom)
        {
            m_CameraPosition.y = playerPos.y - bottom;
        }
        if (playerPos.y > m_CameraPosition.y + top)
        {
            m_CameraPosition.y = playerPos.y + bottom;
        }

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
    }


    // --------------------------------------------
    // Game Logic System
    // --------------------------------------------
    GDGameLogicSystem::GDGameLogicSystem(ECS* ecs) : System(ecs), m_Player(4294967295)
    {
    }

    void GDGameLogicSystem::OnUpdate(Timestep ts)
    {
        // if the ts is too large, clamp it and slow down the update speed
        ts = std::min((float)ts, 0.033f);


        auto& playerTransform = m_ECS->GetComponent<Transform>(m_Player);
        auto& playerRigidBody = m_ECS->GetComponent<RigidBody>(m_Player);
        auto& playerGravity = m_ECS->GetComponent<Gravity>(m_Player);
        auto& playerProps = m_ECS->GetComponent<GDPlayer>(m_Player);

        // check player death
        static float timeElapsed = 0.0f;
        if (!playerProps.Alive)
        {
            if (timeElapsed == 0.0f)
            {
                m_ECS->GetComponent<GDObject>(m_Player).Visible = false;
                // first frame, emit tons of particles
                for (int i = 0; i < 100; i++)
                {
                    GDParticleSystem::Emit({
                        {playerTransform.Position.x, playerTransform.Position.y, 1.0f},
                        {0.0f, 0.0f},
                        {7.5f, 7.5f},
                        {1.0f, 0.92f, 0.29f, 1.0f},
                        {1.0f, 0.52f, 0.29f, 0.0f},
                        0.2f,
                        0.05f,
                        0.05f,
                        0.9f
                    });
                }
            }
            timeElapsed += ts;

            return; // the game logic system will stop updating once the player is dead
        }
        else
        {
            timeElapsed = 0.0f;
        }

        // physics logic
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
                playerRigidBody.Velocity.x = 15.0f;
            }
            else if (Input::IsKeyPressed(HZ_KEY_LEFT))
            {
                playerRigidBody.Velocity.x = -15.0f;
            }
            else
            {
                playerRigidBody.Velocity.x = 0.0f;
            }

            // keep rotation within range
            if (playerTransform.Rotation >= 360.0f)
            {
                playerTransform.Rotation -= 360.0f;
            }
            if (playerTransform.Rotation < 0.0f)
            {
                playerTransform.Rotation += 360.0f;
            }

            // jumping rotation
            if (!playerProps.OnTheGround)
            {
                if (playerRigidBody.Velocity.x >= 0.0f)
                {
                    playerTransform.Rotation -= 360 * ts;
                }
                else
                {
                    playerTransform.Rotation += 360 * ts;
                }
            }
            else
            {
                // go towards the nearest non-rotated position
                static float angles[] = {0.0f, 90.0f, 180.0f, 270.0f};
                float smallest = 360.0f;
                int smallestIndex = -1;
                for (int i = 0; i < 4; i++)
                {
                    float distance = std::abs(playerTransform.Rotation - angles[i]);
                    if (distance < smallest)
                    {
                        smallest = distance;
                        smallestIndex = i;
                    }
                }
                float targetAngle = angles[smallestIndex];
                if (smallest < 5.0f)
                {
                    playerTransform.Rotation = targetAngle;
                }
                else
                {
                    playerTransform.Rotation -= (playerTransform.Rotation - targetAngle) * ts * 20;
                }

                // player particle effect on the ground
                auto& playerPos = playerTransform.Position;
                for (int i = 0; i < 8; i++)
                {
                    GDParticleSystem::Emit({
                        {playerPos.x, playerPos.y - 0.45f, playerPos.z}, glm::vec2(0.0f),
                        glm::vec2(2.0f), {1.0f, 0.82f, 0.0f, 1.0f}, {1.0f, 0.55f, 0.0f, 0.5f}, 0.1f, 0.01f,
                        0.03f, 0.5f
                    });
                }
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
            auto& objectProps = m_ECS->GetComponent<GDObject>(e);
            auto& objectTransform = m_ECS->GetComponent<Transform>(e);
            if (objectProps.ObjectType == GDObjectType::Background)
            {
                objectTransform.Position += glm::vec3{
                    playerRigidBody.Velocity.x * ts / 1.1f,
                    0.0f,
                    0.0f
                };
                continue;
            }

            // TODO : support rotation
            // TODO : support trangular slope

            // check if it is in range, if not, don't bother with it
            if (playerTransform.Position.x + playerTransform.Size.x / 2 < objectTransform.Position.x - objectTransform
                                                                                                       .Size.x / 2 ||
                playerTransform.Position.x - playerTransform.Size.x / 2 > objectTransform.Position.x + objectTransform
                                                                                                       .Size.x / 2)
            {
                continue;
            }

            switch (objectProps.ObjectType)
            {
            case GDObjectType::Square:
            {
                // top
                float playerBottomToObjectTop = (playerTransform.Position.y - playerTransform.Size.y / 2) - (
                    objectTransform.Position.y + objectTransform.Size.y / 2);
                if (playerBottomToObjectTop > playerTransform.Size.y / 10.0f)
                {
                    // above
                    break;
                }

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
                float objectBottomToPlayerTop = (objectTransform.Position.y - objectTransform.Size.y / 2) - (
                    playerTransform.Position.y + playerTransform.Size.y / 2);
                if (objectBottomToPlayerTop >= 0.0f)
                {
                    // below the block
                    // continue on
                    break;
                }

                // kill
                playerProps.Alive = false;
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
            case GDObjectType::Triangle:
            {
                const auto& playerPosition = playerTransform.Position;
                const auto& trianglePosition = objectTransform.Position;
#if 0
                float distance = glm::distance(glm::vec2{playerPosition.x, playerPosition.y},
                                               glm::vec2{trianglePosition.x, trianglePosition.y});
                if (distance > playerTransform.Size.x / 2 + objectTransform.Size.x / 2)
                {
                    // not colliding
                    break;
                }
#endif

                bool collide = false;
                // compute the three points
                glm::vec2 pt1 = {
                    trianglePosition.x,
                    trianglePosition.y + objectTransform.Size.y / 2
                };
                glm::vec2 pt2 = {
                    trianglePosition.x - objectTransform.Size.x / 2,
                    trianglePosition.y - objectTransform.Size.y / 2
                };
                glm::vec2 pt3 = {
                    trianglePosition.x + objectTransform.Size.x / 2,
                    trianglePosition.y - objectTransform.Size.y / 2
                };

                // player coords
                glm::vec2 playerPts[4] = {
                    {playerPosition.x + playerTransform.Size.x / 2, playerPosition.y + playerTransform.Size.y / 2},
                    {playerPosition.x - playerTransform.Size.x / 2, playerPosition.y + playerTransform.Size.y / 2},
                    {playerPosition.x - playerTransform.Size.x / 2, playerPosition.y - playerTransform.Size.y / 2},
                    {playerPosition.x + playerTransform.Size.x / 2, playerPosition.y - playerTransform.Size.y / 2}
                };

                for (int i = 0; i < 4; i++)
                {
                    if (IsPointInTriangle(playerPts[i], pt1, pt2, pt3))
                    {
                        collide = true;
                        break;
                    }
                }

                // kill
                if (objectProps.Killer && collide)
                {
                    playerProps.Alive = false;
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
