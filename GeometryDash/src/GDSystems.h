#pragma once
#include "Hazel/ECS/Systems.h"
#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel {
    class WindowResizeEvent;
    class MouseScrolledEvent;
}

using namespace Hazel;

namespace GD
{
    class CameraSystem : public System
    {
    public:
        explicit CameraSystem(ECS* ecs);

        void OnEvent(Event& event) override;

        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);

        void OnUpdate(Timestep ts) override;
        void OnEntityAdded(Entity e) override;
        void OnEntityRemoved(Entity e) override;

        OrthographicCamera& GetCamera()
        {
            return m_Camera;
        }

    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;
        bool m_Rotation;
        float m_CameraRotation = 0.0f;
        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 180.0f;
    };
}
