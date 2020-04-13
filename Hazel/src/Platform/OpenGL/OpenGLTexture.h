﻿#pragma once
#include "Hazel/Renderer/Texture.h"

namespace Hazel {
    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(const std::string& path);
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

    private:
        std::string m_Path;
        unsigned int m_Width, m_Height;
        unsigned int m_RendererID;
    };

}