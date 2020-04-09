#pragma once

#include "Core.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer/Shader.h"
#include "Window.h"

namespace Hazel
{
    class Layer;
    class WindowCloseEvent;

    class HAZEL_API Application
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

        unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
        std::unique_ptr<Shader> m_Shader;

        static Application* s_Instance;
    };

    // To be defined in client
    Application* CreateApplication();
}
