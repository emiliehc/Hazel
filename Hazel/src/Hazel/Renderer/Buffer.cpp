#include "hzpch.h"
#include "Buffer.h"


#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Renderer.h"

namespace Hazel
{
    VertexBuffer* VertexBuffer::Create(float* vertices, unsigned size)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::OpenGL:
        {
            return new OpenGLVertexBuffer(vertices, size);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(unsigned int* indices, unsigned count)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::OpenGL:
        {
            return new OpenGLIndexBuffer(indices, count);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
}
