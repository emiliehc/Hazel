#include "hzpch.h"
#include "Renderer2D.h"

#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Hazel/Debug/Instrumentor.h"

namespace Hazel
{
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        // TODO : texid
    };

    struct Renderer2DData
    {
        const unsigned MaxQuads = 10000;
        const unsigned MaxVertices = MaxQuads * 4;
        const unsigned MaxIndices = MaxQuads * 6;


        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;

        unsigned QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        HZ_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = VertexArray::Create();
        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

        s_Data.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"}
        });
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];


        auto* quadIndices = new unsigned[s_Data.MaxIndices];

        unsigned offset = 0;
        for (unsigned i = 0; i < s_Data.MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

        delete[] quadIndices;


        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        unsigned whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(unsigned));

        s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetInt("u_Texture", 0);
    }

    void Renderer2D::ShutDown()
    {
        HZ_PROFILE_FUNCTION();
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::EndScene()
    {
        HZ_PROFILE_FUNCTION();

        unsigned long long dataSize = (unsigned char*)s_Data.QuadVertexBufferPtr - (unsigned char*)s_Data.
            QuadVertexBufferBase;
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer2D::Flush()
    {
        HZ_PROFILE_FUNCTION();

        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
    }

    /*
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                     const glm::vec4& color)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                     const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", color);
        s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data.WhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);
        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                     const Ref<Texture2D>& texture, float tilingFactor)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                     const Ref<Texture2D>& texture, float tilingFactor)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);


        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                     const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tint, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                     const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", tint);
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);

        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);


        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }

*/
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }


    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.QuadVertexBufferPtr->Position = position;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = {0.0f, 0.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = {1.0f, 0.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, position.z};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = {1.0f, 1.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, position.z};;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = {0.0f, 1.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;

        /*
        s_Data.TextureShader->SetFloat4("u_Color", color);
        s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data.WhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);
        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
        */
    }

    /*
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              float tilingFactor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              float tilingFactor)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);


        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              const glm::vec4& tint, float tilingFactor)
    {
        DrawQuad({position.x, position.y, 1.0f}, size, texture, tint, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              const glm::vec4& tint, float tilingFactor)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", tint);
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);

        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Transform", transform);


        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }
    */
}
