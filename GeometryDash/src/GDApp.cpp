#include "GDAssetManager.h"
#include "GDLayer.h"
#include "Hazel.h"
#include "Hazel/Core/EntryPoint.h"
#include "imgui/imgui.h"


namespace GD
{
    class GDApplication : public Application
    {
    public:
        GDApplication()
        {
            PushLayer(new GDLayer);
        }

        ~GDApplication() override = default;
    };
}


Application* Hazel::CreateApplication()
{
    // init asset manager
    return new GD::GDApplication;
}
