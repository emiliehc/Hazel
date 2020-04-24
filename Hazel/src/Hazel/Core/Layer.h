#pragma once
#include "hzpch.h"

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/ECS/ECSCore.h"

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
            m_ECS.OnUpdate(ts);
        }

        virtual void OnImGuiRender()
        {
        }

        virtual void OnEvent(Event& event)
        {
            m_ECS.OnEvent(event);
        }

        const std::string& GetName() const
        {
            return m_DebugName;
        }

    protected:
        std::string m_DebugName;
        ECS m_ECS;
    };
}
