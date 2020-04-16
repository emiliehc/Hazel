// ReSharper disable All
#pragma once
#include "Application.h"
#include "Log.h"

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
    Hazel::Log::Init();
    HZ_CORE_WARN("Initialized Log!");
    auto a = 5;
    HZ_INFO("Hello! Var={0}", a);

    auto app = Hazel::CreateApplication();
    app->Run();
    delete app;
}


#endif
