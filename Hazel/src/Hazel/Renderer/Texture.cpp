#include "hzpch.h"
#include "Texture.h"


#include "Platform/OpenGL/OpenGLTexture.h"
#include "Renderer.h"

namespace Hazel
{
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return std::make_shared<OpenGLTexture2D>(path);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
}
