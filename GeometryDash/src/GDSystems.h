#pragma once
#include "Hazel/ECS/Components.h"
#include "Hazel/ECS/Systems.h"
#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel
{
    class WindowResizeEvent;
    class MouseScrolledEvent;
}

using namespace Hazel;

namespace GD
{
    // custom 2d renderer
    class GDRendererSystem : public System
    {
    public:
        explicit GDRendererSystem(ECS* ecs);

        Ref<System> Clone() const override;

        ~GDRendererSystem() override = default;

        void OnUpdate(Timestep ts) override;

        void SetAccentColor(const glm::vec4& accentColor)
        {
            m_AccentColor = accentColor;
        }

    private:
        glm::vec4 m_AccentColor = {0.0f, 0.3176470588f, 1.0f, 1.0f};
    };

    class GDCameraSystem : public System
    {
    public:
        explicit GDCameraSystem(ECS* ecs);

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

        Entity GetPlayer() const
        {
            return m_Player;
        }

        void SetPlayer(Entity e);

        Ref<System> Clone() const override
        {
            return std::static_pointer_cast<System>(std::make_shared<GDCameraSystem>(*this));
        }

    private:
        // player
        Entity m_Player;
        Transform* m_PlayerTransform;

        // camera
        float m_AspectRatio;
        float m_ZoomLevel = 6.0f;
        OrthographicCamera m_Camera;
        bool m_Rotation;
        float m_CameraRotation = 0.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 180.0f;
    };

    class GDGameLogicSystem : public System
    {
    public:
        explicit GDGameLogicSystem(ECS* ecs);

        void OnUpdate(Timestep ts) override;
        void OnEvent(Event& event) override;
        void OnEntityAdded(Entity e) override;
        void OnEntityRemoved(Entity e) override;

        Entity GetPlayer() const
        {
            return m_Player;
        }

        void SetPlayer(Entity e)
        {
            m_Entities.erase(e);
            m_Player = e;
        }

        Ref<System> Clone() const override
        {
            return std::static_pointer_cast<System>(std::make_shared<GDGameLogicSystem>(*this));
        }

        void SetAccentColor(const glm::vec4& accentColor);

        glm::vec4 GetAccentColor() const
        {
            return m_AccentColor;
        }

    private:
        Entity m_Player;
        glm::vec4 m_AccentColor = {0.0f, 0.3176470588f, 1.0f, 1.0f};
        // the accent color will be used if no custom color is specified
    };
}
