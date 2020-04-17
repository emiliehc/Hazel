#pragma once
#include "glm/mat4x4.hpp"
#include "Hazel/Renderer/Shader.h"

// TODO : remove
typedef unsigned int GLenum;

namespace Hazel
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        OpenGLShader(const std::string& filepath);
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

        const std::string& GetName() const override
        {
            return m_Name;
        }
    private:
        unsigned int m_RendererID;
        std::string m_Name;

        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
    public:
        void SetMat4(const std::string& name, const glm::mat4& values) override;
        void SetFloat4(const std::string& name, const glm::vec4& values) override;
        void SetFloat3(const std::string& name, const glm::vec3& values) override;
    };
}
