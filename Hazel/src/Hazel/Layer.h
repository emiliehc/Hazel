#pragma once
#include "hzpch.h"

#include "Core/Timestep.h"

namespace Hazel
{
    class Event;

    class HAZEL_API Layer
    {
    public:
        explicit Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach()
        {
        }

        virtual void OnDetach()
        {
        }

        virtual void OnUpdate(Timestep ts)
        {
        }

        virtual void OnImGuiRender()
        {
        }

        virtual void OnEvent(Event& event)
        {
        }

        const std::string& GetName() const
        {
            return m_DebugName;
        }

    protected:
        std::string m_DebugName;
    };
}
