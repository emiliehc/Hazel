#pragma once
#include "glm/glm.hpp"

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

        // primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

    };
}
