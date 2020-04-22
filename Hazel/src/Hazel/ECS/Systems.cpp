#include "hzpch.h"
#include "Systems.h"

namespace Hazel
{
    PhysicsSystem::PhysicsSystem()
    {
    }

    void PhysicsSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities)
        {
            // TODO
        }
    }
}
