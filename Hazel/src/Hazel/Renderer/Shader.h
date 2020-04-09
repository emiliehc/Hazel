#pragma once
#include <string>

namespace Hazel
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~Shader();

        void Bind() const;
        void Unbind() const;
    private:
        unsigned int m_RendererID;
    };
}
