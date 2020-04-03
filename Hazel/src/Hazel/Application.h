#pragma once

#include "Core.h"
#include "Window.h"

namespace Hazel
{
    class WindowCloseEvent;

    class HAZEL_API Application
    {
    public:
        Application();
        virtual ~Application();
        
        void Run();

        void OnEvent(Event& e);

    private:
        bool OnWindowClosed(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    // To be defined in client
    Application* CreateApplication();
}


