#pragma once

#include "Core.h"
#include "LayerStack.h"
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
        bool m_Running = true;
        LayerStack m_LayerStack;
        static Application* s_Instance;
    };

    // To be defined in client
    Application* CreateApplication();
}
