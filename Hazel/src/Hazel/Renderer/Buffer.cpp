#include "hzpch.h"
#include "Buffer.h"


#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Renderer.h"

namespace Hazel
{
    Ref<VertexBuffer> VertexBuffer::Create(unsigned size)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return std::make_shared<OpenGLVertexBuffer>(size);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, unsigned size)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return std::make_shared<OpenGLVertexBuffer>(vertices, size);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
        
    Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, unsigned count)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return std::make_shared<OpenGLIndexBuffer>(indices, count);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
}
