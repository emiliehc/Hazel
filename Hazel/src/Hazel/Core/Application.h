#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Window.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);

        void PushOverlay(Layer* layer);

        static Application& Get()
        {
            return *s_Instance;
        }

        Window& GetWindow() const
        {
            return *m_Window;
        }

    private:
        bool OnWindowClosed(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        bool m_Minimized = false;
        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;
    };

    // To be defined in client
    Application* CreateApplication();
}
