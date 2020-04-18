#include "hzpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Debug/Instrumentor.h"


namespace Hazel
{
    static GLenum ShaderTypeFromString(const std::string& type)
    {
        HZ_PROFILE_FUNCTION();

        if (type == "vertex")
        {
            return GL_VERTEX_SHADER;
        }
        if (type == "fragment" || type == "pixel")
        {
            return GL_FRAGMENT_SHADER;
        }
        HZ_CORE_ASSERT(false, "Unknown shader type");
        return 0;
    }


    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc,
                               const std::string& fragmentSrc) : m_Name(name)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::OpenGLShader(const std::string& filepath)
    {
        HZ_PROFILE_FUNCTION();

        std::string source = ReadFile(filepath);
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);

        // extract name from file path
        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filepath.rfind('.');
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
    }

    OpenGLShader::~OpenGLShader()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);
    }


    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        HZ_PROFILE_FUNCTION();

        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else
        {
            HZ_CORE_ERROR("Could not open file '{0}'", filepath);
        }
        return result;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        unsigned long long typeTokenLength = strlen(typeToken);
        unsigned long long pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            unsigned long long eol = source.find_first_of("\r\n", pos);
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            unsigned long long begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specifier");

            unsigned long long nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] =
                source.substr(nextLinePos,
                              pos - (nextLinePos == std::string::npos
                                         ? source.size() - 1
                                         : nextLinePos));
        }

        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        HZ_PROFILE_FUNCTION();

        const unsigned int program = glCreateProgram();
        HZ_CORE_ASSERT(shaderSources.size() <= 5, "Too many shaders");
        std::array<GLenum, 5> glShaderIDs;
        int glShaderIDIndex = 0;
        for (auto& kv : shaderSources)
        {
            GLenum shaderType = kv.first;
            const std::string& source = kv.second;

            const unsigned int shader = glCreateShader(shaderType);

            const char* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, nullptr);

            glCompileShader(shader);

            int isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                int maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<char> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                HZ_CORE_ERROR("{0}", infoLog.data());
                HZ_CORE_ASSERT(false, "Shader compilation failure");

                break;
            }

            glAttachShader(program, shader);
            glShaderIDs[glShaderIDIndex++] = shader;
        }

        glLinkProgram(program);

        int isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            int maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);

            for (auto id : glShaderIDs)
            {
                glDeleteShader(id);
            }

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader linking failure");

            // In this simple program, we'll just leave
            return;
        }

        for (auto id : glShaderIDs)
        {
            glDetachShader(program, id);
        }

        m_RendererID = program;
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& values)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformMat4(name, values);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& values)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat4(name, values);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& values)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat3(name, values);
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformInt(name, value);
    }

    void OpenGLShader::Bind() const
    {
        HZ_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        HZ_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform3f(location, values.x, values.y, values.z);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform2f(location, values.x, values.y);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform1i(location, value);
    }

    /**
     * BIND BEFORE UPLOADING
     */
    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform4f(location, values.x, values.y, values.z, values.w);
    }
}
