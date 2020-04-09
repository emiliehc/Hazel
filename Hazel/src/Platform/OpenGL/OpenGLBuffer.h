#pragma once
#include "Hazel/Renderer/Buffer.h"

namespace Hazel
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float* vertices, unsigned int size);
        ~OpenGLVertexBuffer() override;
        void Bind() const override;
        void Unbind() const override;
    private:
        unsigned int m_RendererID;
    };

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
        ~OpenGLIndexBuffer() override;
        unsigned int GetCount() const override;
        void Bind() const override;
        void Unbind() const override;
    private:
        unsigned int m_RendererID;
        unsigned int m_Count;
    };
}
