#include "hzpch.h"
#include "Systems.h"
#include "ECSCore.h"

namespace Hazel
{
    PhysicsSystem::PhysicsSystem(ECS* ecs) : System(ecs)
    {
    }

    void PhysicsSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities)
        {
            // TODO
        }
    }

    RendererSystem::RendererSystem(ECS* ecs) : System(ecs)
    {
    }

    void RendererSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities) {
            // render
            
        }
    }
}
