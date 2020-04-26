#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
    class Timestep;

    struct ParticleProps
    {
        glm::vec3 Position;
        glm::vec2 Velocity, VelocityVariation;
        glm::vec4 ColorBegin, ColorEnd;
        float SizeBegin, SizeEnd, SizeVariation;
        float LifeTime = 1.0f;
    };

    class ParticleSystem
    {
    public:
        ParticleSystem();
        virtual ~ParticleSystem();

        void OnUpdate(Timestep ts);
        void Emit(const ParticleProps& particleProps);

    private:
        struct Particle {
            glm::vec3 Position;
            glm::vec2 Velocity;
            glm::vec4 ColorBegin, ColorEnd;
            float Rotation = 0.0f;
            float SizeBegin, SizeEnd;

            float LifeTime = 1.0f;
            float LifeRemaining = 0.0f;

            bool Active = false;
        };

        std::vector<Particle> m_ParticlePool;
        unsigned m_PoolIndex = 999;
    };
}
