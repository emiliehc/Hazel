#include "hzpch.h"
#include "OpenGLTexture.h"


#include "glad/glad.h"
#include "stb_image.h"
#include "Hazel/Debug/Instrumentor.h"

namespace Hazel
{
    OpenGLTexture2D::OpenGLTexture2D(unsigned width, unsigned height) : m_Width(width), m_Height(height)
    {
        HZ_PROFILE_FUNCTION();

        m_InternalFormat = GL_RGB32F;
        m_DataFormat = GL_RGB;

        HZ_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path)
    {
        HZ_PROFILE_FUNCTION();

        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = nullptr;
        {
            HZ_PROFILE_SCOPE("stbi_load");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        HZ_CORE_ASSERT(data, "Failed to laod image!");
        m_Width = width;
        m_Height = height;

        unsigned int internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA32F;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB32F;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);


        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(unsigned int slot) const
    {
        HZ_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTexture2D::SetData(void* data, unsigned size)
    {
        HZ_PROFILE_FUNCTION();

        unsigned bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        HZ_CORE_ASSERT(bpp == m_Width * m_Height * bpp, "Data must be entire texture");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }
}
