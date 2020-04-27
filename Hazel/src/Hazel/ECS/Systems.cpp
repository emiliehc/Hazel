#include "hzpch.h"
#include "Systems.h"

#include "Components.h"
#include "ECSCore.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel
{
  

    // -----------------------------------------------------------------
    // Renderer System
    // -----------------------------------------------------------------
    RendererSystem::RendererSystem(ECS* ecs) : System(ecs)
    {
    }

    Ref<System> RendererSystem::Clone() const
    {
        return std::static_pointer_cast<System>(std::make_shared<RendererSystem>(*this));
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
