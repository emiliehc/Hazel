#pragma once
#include "glm/glm.hpp"
#include "Texture.h"

namespace Hazel
{
    class OrthographicCamera;

    class Renderer2D
    {
    private:
        Renderer2D() = delete;
    public:
        static void Init();
        static void ShutDown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene(); // probably won't do anything except in a batch renderer
        static void Flush();

        // primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

        struct Statistics {
            unsigned DrawCalls = 0;
            unsigned QuadCount = 0;

            unsigned GetTotalVertexCount() {
                return QuadCount * 4;
            }

            unsigned GetTotalIndexCount() {
                return QuadCount * 6;
            }
        };
        static Statistics GetStats();
        static void ResetStates();
    private:
        static void FlushAndReset();
    };
}
