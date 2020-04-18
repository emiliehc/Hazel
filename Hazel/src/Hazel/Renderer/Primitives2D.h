#pragma once
#include "glm/glm.hpp"

namespace Hazel
{
    class Quad
    {
    public:
        Quad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color,
             const Ref<Texture2D>& texture, float tilingFactor = 1.0f,
             float rotation = 0.0f) : m_Position(position),
                                      m_Size(size),
                                      m_Rotation(rotation),
                                      m_Color(color),
                                      m_Texture(texture),
                                      m_TilingFactor(tilingFactor),
                                      m_HasRotation(rotation != 0.0f),
                                      m_HasTexture(true)
        {
        }

        Quad(const glm::vec3& position, const glm::vec2& size,
             const Ref<Texture2D>& texture, float tilingFactor = 1.0f,
             float rotation = 0.0f) : m_Position(position),
                                      m_Size(size),
                                      m_Rotation(rotation),
                                      m_Color(1.0f, 1.0f, 1.0f, 1.0f),
                                      m_Texture(texture),
                                      m_TilingFactor(tilingFactor),
                                      m_HasRotation(rotation != 0.0f),
                                      m_HasTexture(true)
        {
        }

        Quad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color,
             float rotation = 0.0f) : m_Position(position),
                                      m_Size(size),
                                      m_Rotation(rotation),
                                      m_Color(color),
                                      m_Texture(nullptr),
                                      m_TilingFactor(1.0f),
                                      m_HasRotation(rotation != 0.0f),
                                      m_HasTexture(false)
        {
        }

    private:
        glm::vec3 m_Position;
        glm::vec2 m_Size;
        float m_Rotation;
        glm::vec4 m_Color;
        Ref<Texture2D> m_Texture;
        float m_TilingFactor;

    public:
        [[nodiscard]] const glm::vec3& GetPosition() const
        {
            return m_Position;
        }

        void SetPosition(const glm::vec3& position)
        {
            m_Position = position;
        }

        void SetPosition(const glm::vec2& position)
        {
            m_Position = glm::vec3(position, 0.0f);
        }

        [[nodiscard]] const glm::vec2& GetSize() const
        {
            return m_Size;
        }

        void SetSize(const glm::vec2& size)
        {
            m_Size = size;
        }

        [[nodiscard]] float GetRotation() const
        {
            return m_Rotation;
        }

        void SetRotation(float rotation)
        {
            m_HasRotation = rotation != 0.0f;
            m_Rotation = rotation;
        }

        [[nodiscard]] const glm::vec4& GetColor() const
        {
            return m_Color;
        }

        void SetColor(const glm::vec4& color)
        {
            m_Color = color;
        }

        [[nodiscard]] Ref<Texture2D> GetTexture() const
        {
            return m_Texture;
        }

        void SetTexture(const Ref<Texture2D>& texture = Ref<Texture2D>(nullptr))
        {
            m_HasTexture = texture.operator->() != nullptr;
            m_Texture = texture;
        }

        [[nodiscard]] float GetTilingFactor() const
        {
            return m_TilingFactor;
        }

        void SetTilingFactor(float tilingFactor)
        {
            m_TilingFactor = tilingFactor;
        }

        bool HasRotation() const
        {
            return m_HasRotation;
        }

        bool HasTexture() const
        {
            return m_HasTexture;
        }

    private:
        bool m_HasRotation;
        bool m_HasTexture;
    };
}
