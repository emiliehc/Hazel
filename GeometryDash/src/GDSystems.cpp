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
#include "Hazel/Events/KeyEvent.h"
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
        m_CameraPosition.y = std::max(m_CameraPosition.y, 2.87166715f);

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

    static bool s_SpaceBarPressed = false;

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
                playerRigidBody.Velocity.y = 27.0f;
            }

#if defined(HZ_DEBUG) || defined(HZ_RELEASE)
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
#endif

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
        playerRigidBody.Velocity.y = std::max(playerRigidBody.Velocity.y, -30.0f);
        playerRigidBody.Velocity.y = std::min(playerRigidBody.Velocity.y, 50.0f);
        // TODO : these limit literals will be extracted later into private fields for better access
        playerTransform.Position += playerRigidBody.Velocity * ts.GetSeconds();

        if (Input::IsKeyPressed(HZ_KEY_B))
        {
            HZ_ASSERT(false, "");
        }

        // do not let the player go outside of its designated area
        if (playerTransform.Position.y < -2.0f || playerTransform.Position.y > 50.0f)
        {
            playerProps.Alive = false;
        }

        // always do game logic update before this
        playerProps.OnTheGround = false; // default
        // reset its position if a collision is detected

        const auto& cameraPos = m_ECS->GetSystem<GDCameraSystem>()->GetCamera().GetPosition();
        for (Entity e : m_Entities)
        {
            auto& objectProps = m_ECS->GetComponent<GDObject>(e);
            auto& objectTransform = m_ECS->GetComponent<Transform>(e);

            if (objectProps.ObjectType == GDObjectType::Background)
            {
                objectTransform.Position += glm::vec3{
                    playerRigidBody.Velocity.x * ts / 1.08f,
                    0.0f,
                    0.0f
                };
                objectTransform.Position.y = cameraPos.y / 1.08f;
                continue;
            }

            // TODO : support rotation
            // TODO : support trangular slope

            // check if it is in range, if not, don't bother with it, except for triggers
            if (objectProps.ObjectType != GDObjectType::Trigger)
            {
                if (playerTransform.Position.x + playerTransform.Size.x / 2 < objectTransform.Position.x -
                    objectTransform
                    .Size.x / 2 ||
                    playerTransform.Position.x - playerTransform.Size.x / 2 > objectTransform.Position.x +
                    objectTransform
                    .Size.x / 2)
                {
                    continue;
                }
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
            case GDObjectType::Trigger:
            {
                const auto& triggerObjectProps = objectTransform;
                auto& triggerProps = m_ECS->GetComponent<GDTrigger>(e);
                if (triggerProps.Cycle == GDTriggerCycle::Triggered)
                {
                    triggerProps.ElapsedTime += ts;
                    if (triggerProps.ElapsedTime > triggerProps.Duration)
                    {
                        // reset trigger
                        triggerProps.ElapsedTime = 0.0f;
                        triggerProps.Cycle = GDTriggerCycle::Ready;
                        switch (triggerProps.Type)
                        {
                        case GDTriggerType::Color:
                        {
                            const auto& colorTriggerProps = triggerProps.ColorTriggerProps;
                            if (colorTriggerProps.AffectsAccentColor)
                            {
                                SetAccentColor(colorTriggerProps.TargetColor);
                            }
                            else
                            {
                                for (const Entity object : m_GroupIDToEntitiesMap[triggerProps.ObjectGroupID])
                                {
                                    if (m_ECS->HasComponent<Colored>(object))
                                    {
                                        m_ECS->GetComponent<Colored>(object).Color = colorTriggerProps.TargetColor;
                                    }
                                }
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                        break;
                    }

                    switch (triggerProps.Type)
                    {
                    case GDTriggerType::Move:
                    {
                        const auto& moveTriggerProps = triggerProps.MoveTriggerProps;
                        for (const Entity object : m_GroupIDToEntitiesMap[triggerProps.ObjectGroupID])
                        {
                            auto& affectedObjectTransform = m_ECS->GetComponent<Transform>(object);
                            affectedObjectTransform.Position += moveTriggerProps.DeltaPosition / triggerProps.Duration *
                                (float)ts;
                        }

                        break;
                    }
                    case GDTriggerType::Color:
                    {
                        const auto& colorTriggerProps = triggerProps.ColorTriggerProps;
                        if (colorTriggerProps.AffectsAccentColor)
                        {
                            glm::vec4 originalColor = colorTriggerProps.TargetColor - (colorTriggerProps.TargetColor -
                                m_AccentColor) / (1.0f - triggerProps.ElapsedTime / triggerProps.Duration);
                            glm::vec4 difference = colorTriggerProps.TargetColor - originalColor;
                            SetAccentColor(m_AccentColor + difference / triggerProps.Duration * (float)ts);
                        }
                        else
                        {
                            for (const auto object : m_GroupIDToEntitiesMap[triggerProps.ObjectGroupID])
                            {
                                if (m_ECS->HasComponent<Colored>(object))
                                {
                                    auto& objectColor = m_ECS->GetComponent<Colored>(object).Color;
                                    glm::vec4 originalColor = colorTriggerProps.TargetColor - (colorTriggerProps.
                                        TargetColor - objectColor) / (1.0f - triggerProps.ElapsedTime / triggerProps.
                                        Duration);
                                    glm::vec4 difference = colorTriggerProps.TargetColor - originalColor;
                                    objectColor += difference / triggerProps.Duration * (float)ts;
                                }
                            }
                        }

                        break;
                    }
                    default:
                    {
                        HZ_ASSERT(false, "Unsupported trigger type!");
                    }
                    }
                }
                else
                {
                    // calc dx for triggers that are ready
                    float dx = playerTransform.Position.x - triggerObjectProps.Position.x;
                    if (dx >= 0.0f && dx < 0.4f)
                    {
                        triggerProps.Cycle = GDTriggerCycle::Triggered;
                    }
                }

                break;
            }
            case GDObjectType::JumpRing:
            {
                const auto& jumpRingProps = m_ECS->GetComponent<GDJumpRingProps>(e);
                float distance = glm::distance(
                    glm::vec2{objectTransform.Position.x, objectTransform.Position.y},
                    glm::vec2{playerTransform.Position.x, playerTransform.Position.y}
                );
                if (distance > objectTransform.Size.x / 2 + playerTransform.Size.x / 2)
                {
                    // not touching
                    break;
                }

                switch (jumpRingProps.Type)
                {
                case GDJumpRingType::Yellow:
                {
                    if (s_SpaceBarPressed)
                    {
                        playerRigidBody.Velocity.y = playerRigidBody.Acceleration.y < 0 ? 35.0f : -35.0f;
                    }
                    
                    break;
                }
                default:
                {
                    HZ_ASSERT(false, "Unsupported jump ring type!");
                    break;
                }
                }
                break;
            }
            default:
            {
                HZ_ASSERT(false, "Unsupported object type!");
            }
            }
        }
        s_SpaceBarPressed = false;
    }

    void GDGameLogicSystem::OnEvent(Event& event)
    {
        // once a space bar event is received, it sends a signal to the on update functino via some kind of shared variable (for single threaded design only)
        // get key repeat count. if the repeat count is 1 or greater, ignore it, because that will be managed in the OnUpdate function
        EventDispatcher dispatcher(event);
        std::function<bool(KeyPressedEvent&)> callback = [](KeyPressedEvent& e) -> bool
        {
            if (e.GetKeyCode() == HZ_KEY_SPACE && e.GetRepeatCount() == 0)
            {
                s_SpaceBarPressed = true;
            }
            return false;
        };
        dispatcher.Dispatch<KeyPressedEvent>(callback);
    }

    void GDGameLogicSystem::OnEntityAdded(Entity e)
    {
        if (e == m_Player)
        {
            m_Entities.erase(e);
        }
        else
        {
            m_GroupIDToEntitiesMap[m_ECS->GetComponent<GDObject>(e).GroupID].insert(e);
        }
    }

    void GDGameLogicSystem::OnEntityRemoved(Entity e)
    {
        if (m_ECS->HasComponent<GDObject>(e))
        {
            m_GroupIDToEntitiesMap[m_ECS->GetComponent<GDObject>(e).GroupID].erase(e);
        }
    }

    void GDGameLogicSystem::SetAccentColor(const glm::vec4& accentColor)
    {
        m_AccentColor = accentColor;
        m_ECS->GetSystem<GDRendererSystem>()->SetAccentColor(accentColor);
    }
}
