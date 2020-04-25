#include "hzpch.h"
#include "ECSCore.h"
#include "Components.h"

namespace Hazel
{
    ECS::ECS(): m_ComponentManager(CreateScope<ComponentManager>()),
                m_EntityManager(CreateScope<EntityManager>()),
                m_SystemManager(CreateScope<SystemManager>())
    {
    }
}
