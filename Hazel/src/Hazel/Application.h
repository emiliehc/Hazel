#pragma once

#include "Core/Timestep.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Window.h"

namespace Hazel
{
    class Layer;
    class WindowCloseEvent;

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

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;
    };

    // To be defined in client
    Application* CreateApplication();
}
