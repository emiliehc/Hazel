#include "hzpch.h"
#include "Layer.h"

#include "Hazel/ECS/Components.h"
#include "Hazel/ECS/Systems.h"

namespace Hazel
{
    Layer::Layer(const std::string& name) : m_DebugName(name)
    {
        m_ECS.RegisterComponent<Gravity>();
        m_ECS.RegisterComponent<Transform>();
        m_ECS.RegisterComponent<RigidBody>();

        m_ECS.RegisterSystem<PhysicsSystem>();
        {
            Signature signature;
            signature.set(m_ECS.GetComponentType<Gravity>());
            signature.set(m_ECS.GetComponentType<RigidBody>());
            signature.set(m_ECS.GetComponentType<Transform>());
            m_ECS.SetSystemSignature<PhysicsSystem>(signature);
        }

        // repeat for other systems
    }

    Layer::~Layer() = default;
}
