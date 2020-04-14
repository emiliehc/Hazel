#include "hzpch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Hazel
{
    Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return new OpenGLShader(vertexSrc, fragmentSrc);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }

    Shader* Shader::Create(const std::string& filepath)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
        {
            HZ_CORE_ASSERT(false, "Renderer API none is not supported!");
            return nullptr;
        }
        case RendererAPI::API::OpenGL:
        {
            return new OpenGLShader(filepath);
        }
        }
        HZ_CORE_ASSERT(false, "Unknown renderer API!");
        return nullptr;
    }
}
