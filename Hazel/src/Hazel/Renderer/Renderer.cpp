#include "hzpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{
    Renderer::SceneData* Renderer::m_SceneData = new SceneData;

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        *m_SceneData = {camera.GetViewProjectionMatrix()};
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,
                          const glm::mat4& transform)
    {
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
}
