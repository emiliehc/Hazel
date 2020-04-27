#pragma once
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(unsigned width, unsigned height);
        ~OpenGLTexture2D() override;

        unsigned GetWidth() override
        {
            return m_Width;
        }

        unsigned GetHeight() override
        {
            return m_Height;
        }

        void Bind(unsigned int slot = 0) const override;

        void SetData(void* data, unsigned size) override;

        bool operator==(const Texture& other) const override
        {
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }

    private:
        std::string m_Path;
        unsigned int m_Width, m_Height;
        unsigned int m_RendererID;
        unsigned int m_InternalFormat, m_DataFormat;
    };
}
