#include "hzpch.h"
#include "ECSCore.h"
#include "Components.h"

namespace Hazel
{
    ECS::ECS(): m_ComponentManager(CreateScope<ComponentManager>()),
                m_EntityManager(CreateScope<EntityManager>()),
                m_SystemManager(CreateScope<SystemManager>())
    {
        // register components
        RegisterComponent<Gravity>();
        RegisterComponent<Transform>();
        RegisterComponent<RigidBody>();
        RegisterComponent<Colored>();
        RegisterComponent<Textured>();
        RegisterComponent<Drawable>();

        RegisterSystem<PhysicsSystem>();
        {
            Signature signature;
            signature.set(GetComponentType<Gravity>());
            signature.set(GetComponentType<RigidBody>());
            signature.set(GetComponentType<Transform>());
            signature.set(GetComponentType<Drawable>());
            SetSystemSignature<PhysicsSystem>(signature);
        }

        RegisterSystem<RendererSystem>();
        {
            Signature signature;
            signature.set(GetComponentType<Transform>());
            signature.set(GetComponentType<Drawable>());
            signature.set(GetComponentType<Colored>());
            SetSystemSignature<RendererSystem>(signature);
        }

        // repeat for other systems
    }
}
