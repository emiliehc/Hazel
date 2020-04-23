#pragma once

#include "ECSTypeDefs.h"
#include "Hazel/Core/Timestep.h"
#include <set>

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
    public:
        std::set<Entity> m_Entities;
    protected:
        ECS* m_ECS;
    };

    class PhysicsSystem : public System {
    public:
        explicit PhysicsSystem(ECS* ecs);

        ~PhysicsSystem() override = default;

        void OnUpdate(Timestep ts) override;
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
