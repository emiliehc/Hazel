#include "hzpch.h"
#include "Systems.h"

#include "Components.h"
#include "ECSCore.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel
{
    PhysicsSystem::PhysicsSystem(ECS* ecs) : System(ecs)
    {
    }

    void PhysicsSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities)
        {
            // TODO
        }
    }

    RendererSystem::RendererSystem(ECS* ecs) : System(ecs)
    {
    }

    void RendererSystem::OnUpdate(Timestep ts)
    {
        for (const Entity e : m_Entities)
        {
            // render
            // quads
            const auto& transform = m_ECS->GetComponent<Transform>(e);
            const auto& color = m_ECS->GetComponent<Colored>(e).Color;
            auto type = m_ECS->GetComponent<Drawable>(e).GeometryType;
            if (type == PrimitiveGeometryType::Quad)
            {
                // has rotation?
                if (transform.Rotation == 0.0f)
                {
                    // has texture?
                    if (m_ECS->HasComponent<Textured>(e))
                    {
                        const auto& textureData = m_ECS->GetComponent<Textured>(e);
                        Renderer2D::DrawQuad(transform.Position, transform.Size, textureData.Texture,
                                             textureData.TilingFactor, color);
                    }
                    else
                    {
                        Renderer2D::DrawQuad(transform.Position, transform.Size, color);
                    }
                }
                else
                {
                    // has texture?
                    if (m_ECS->HasComponent<Textured>(e))
                    {
                        const auto& textureData = m_ECS->GetComponent<Textured>(e);
                        Renderer2D::DrawRotatedQuad(transform.Position, transform.Size, transform.Rotation,
                                                    textureData.Texture, textureData.TilingFactor, color);
                    }
                    else
                    {
                        Renderer2D::DrawRotatedQuad(transform.Position, transform.Size, transform.Rotation, color);
                    }
                }
            }
            // TODO : triangles
        }
    }
}
