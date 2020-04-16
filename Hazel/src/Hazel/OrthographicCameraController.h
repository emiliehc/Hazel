#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel
{
    class Timestep;

    class OrthographicCameraController {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false); // aspectratio * 2 units

        void OnUpdate(Timestep ts);

        void OnEvent(Event& e);

        OrthographicCamera& GetCamera()
        {
            return m_Camera;
        }

        const OrthographicCamera& GetCamera() const
        {
            return m_Camera;
        }

        void SetZoomLevel(float level)
        {
            m_ZoomLevel = level;
        }

        float GetZoomLevel() const
        {
            return m_ZoomLevel;
        }
    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;
        bool m_Rotation;
        float m_CameraRotation = 0.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 180.0f;

        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);
    };

}
