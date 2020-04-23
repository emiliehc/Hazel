#include "hzpch.h"
#include "Layer.h"

#include "Hazel/ECS/Components.h"
#include "Hazel/ECS/Systems.h"

namespace Hazel
{
    Layer::Layer(const std::string& name) : m_DebugName(name)
    {

    }

    Layer::~Layer() = default;
}
