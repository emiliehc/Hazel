#include "Hazel.h"

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
    }

    ~Sandbox() override
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox;
}