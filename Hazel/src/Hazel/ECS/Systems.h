#pragma once

#include "Components.h"
#include "ECSCore.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel
{
    class PhysicsSystem : public System
    {
    public:
        explicit PhysicsSystem();

        ~PhysicsSystem() override = default;

        void OnUpdate(Timestep ts);
    };
}
