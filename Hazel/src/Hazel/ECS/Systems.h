#pragma once

#include "ECSTypeDefs.h"
#include "Hazel/Core/Timestep.h"
#include <set>

#include "Hazel/Events/Event.h"

namespace Hazel
{
    class ECS;

    class System
    {
    public:
        System(ECS* ecs) : m_ECS(ecs)
        {
        }

        virtual ~System() = default;
        virtual void OnUpdate(Timestep ts) = 0;

        virtual void OnEvent(Event& event)
        {
        }

        virtual void OnEntityAdded(Entity e)
        {
        }

        virtual void OnEntityRemoved(Entity e)
        {
        }


    public:
        std::set<Entity> m_Entities;
    protected:
        ECS* m_ECS;
    };

    // wrapper for the existing 2d renderer
    class RendererSystem : public System
    {
    public:
        explicit RendererSystem(ECS* ecs);

        ~RendererSystem() override = default;

        void OnUpdate(Timestep ts) override;
    };
}
