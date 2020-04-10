#include "hzpch.h"

#include "VertexArray.h"


#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer.h"

namespace Hazel
{
    VertexArray* VertexArray::Create()
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
            return new OpenGLVertexArray;
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
}
