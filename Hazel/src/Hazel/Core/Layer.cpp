#include "hzpch.h"
#include "Layer.h"

#include "Hazel/ECS/Components.h"
#include "Hazel/ECS/Systems.h"

namespace Hazel
{
    Layer::Layer(const std::string& name) : m_DebugName(name)
    {
        // register components
        m_ECS.RegisterComponent<Gravity>();
        m_ECS.RegisterComponent<Transform>();
        m_ECS.RegisterComponent<RigidBody>();
        m_ECS.RegisterComponent<Colored>();
        m_ECS.RegisterComponent<Textured>();
        m_ECS.RegisterComponent<Drawable>();
        m_ECS.RegisterComponent<Health>();

        m_ECS.RegisterSystem<RendererSystem>();
        {
            Signature signature;
            signature.set(m_ECS.GetComponentType<Transform>());
            signature.set(m_ECS.GetComponentType<Drawable>());
            signature.set(m_ECS.GetComponentType<Colored>());
            m_ECS.SetSystemSignature<RendererSystem>(signature);
        }

        // repeat for other systems
    }

    Layer::~Layer() = default;
}
