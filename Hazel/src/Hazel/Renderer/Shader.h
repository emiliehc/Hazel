#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Hazel
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~Shader();

        void Bind() const;
        void Unbind() const;

        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;
    private:
        unsigned int m_RendererID;
    };
}
