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

        const BufferLayout& GetLayout() const override
        {
            return m_Layout;
        }

        void SetLayout(const BufferLayout& layout) override
        {
            m_Layout = layout;
        }

    private:
        unsigned int m_RendererID;
        BufferLayout m_Layout;
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
