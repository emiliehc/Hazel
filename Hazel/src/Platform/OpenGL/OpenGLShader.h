#pragma once
#include "glm/mat4x4.hpp"
#include "Hazel/Renderer/Shader.h"


namespace Hazel
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader() override;

        void Bind() const override;
        void Unbind() const override;

        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformInt(const std::string& name, int value);

    private:
        unsigned int m_RendererID;
    };
}
