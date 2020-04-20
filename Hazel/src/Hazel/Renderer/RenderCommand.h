#pragma once
#include "RendererAPI.h"


namespace Hazel
{
    // render commands DO NOT do multiple things unless they absolutely have to
    // they are just one layer above the actual APIs
    class RenderCommand
    {
    public:
        static void Init() {
            s_RendererAPI->Init();
        }

        static void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);
        }

        static void SetClearColor(const glm::vec4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }

        static void Clear()
        {
            s_RendererAPI->Clear();
        }

        static void DrawIndexed(const Ref<VertexArray>& vertexArray, long long count = 0)
        {
            s_RendererAPI->DrawIndexed(vertexArray, count);
        }
    private:
        static RendererAPI* s_RendererAPI;
    };
}
