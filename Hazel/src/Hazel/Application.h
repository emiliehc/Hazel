#pragma once

#include "Core.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
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

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Shader> m_BlueShader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexArray> m_SquareVA;

        static Application* s_Instance;
    };

    // To be defined in client
    Application* CreateApplication();
}
