#include "GDSystems.h"

#include <glm/glm.hpp>
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
    CameraSystem::CameraSystem(ECS* ecs) : System(ecs), m_AspectRatio(1280.0f / 720.0f), m_Camera(
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

        m_Camera.SetPosition(m_CameraPosition);

        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void CameraSystem::OnEntityAdded(Entity e)
    {
    }

    void CameraSystem::OnEntityRemoved(Entity e)
    {
    }


    // --------------------------------------------
    // Game Logic System
    // --------------------------------------------
    GameLogicSystem::GameLogicSystem(ECS* ecs) : System(ecs), m_Player(4294967295)
    {
    }

    void GameLogicSystem::OnUpdate(Timestep ts)
    {
        // physics logic
        // treat the player as a circle for collision detections

        for (Entity e : m_Entities)
        {
            HZ_TRACE(e);
        }
    }

    void GameLogicSystem::OnEvent(Event& event)
    {
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
