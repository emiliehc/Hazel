#pragma once
#include "Hazel/Renderer/ParticleSystem.h"
#include "Hazel/Core/Timestep.h"

namespace GD
{
    class GDParticleSystem
    {
    public:
        GDParticleSystem() = delete;

        static void OnUpdate(Timestep ts)
        {
            s_ParticleSystem.OnUpdate(ts);
        }

        static void Emit(const ParticleProps& particleProps)
        {
            s_ParticleSystem.Emit(particleProps);
        }

    private:
        inline static ParticleSystem s_ParticleSystem;
    };
}
