#include "hzpch.h"
#include "ParticleSystem.h"

#include "Hazel/Core/Timestep.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>


#include "Renderer2D.h"
#include "Hazel/Utils/Random.h"

namespace Hazel
{
    ParticleSystem::ParticleSystem()
    {
        m_ParticlePool.resize(1000);
    }

    ParticleSystem::~ParticleSystem()
    {
    }

    void ParticleSystem::OnUpdate(Timestep ts)
    {
        for (auto& particle : m_ParticlePool)
        {
            if (!particle.Active)
                continue;

            if (particle.LifeRemaining <= 0.0f)
            {
                particle.Active = false;
                continue;
            }

            particle.LifeRemaining -= ts;
            auto dv = particle.Velocity* (float)ts;
            particle.Position += glm::vec3{dv.x, dv.y, 0.0f};
            particle.Rotation += 0.01f * ts;
        }

        for (auto& particle : m_ParticlePool)
        {
            if (!particle.Active)
                continue;

            // Fade away particles
            float life = particle.LifeRemaining / particle.LifeTime;
            glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
            //color.a = color.a * life;

            float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

            // Render
            Renderer2D::DrawQuad(particle.Position, {size, size}, color);
        }
    }

    void ParticleSystem::Emit(const ParticleProps& particleProps)
    {
        Particle& particle = m_ParticlePool[m_PoolIndex];
        particle.Active = true;
        particle.Position = particleProps.Position;
        particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

        // Velocity
        particle.Velocity = particleProps.Velocity;
        particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
        particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

        // Color
        particle.ColorBegin = particleProps.ColorBegin;
        particle.ColorEnd = particleProps.ColorEnd;

        particle.LifeTime = particleProps.LifeTime;
        particle.LifeRemaining = particleProps.LifeTime;
        particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
        particle.SizeEnd = particleProps.SizeEnd;

        m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
    }
}
