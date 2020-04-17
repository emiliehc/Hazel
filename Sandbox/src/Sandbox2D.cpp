#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{

}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    Hazel::RenderCommand::Clear();


    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

    Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});

    Hazel::Renderer2D::EndScene();

    // std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
    // std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
    // Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
    m_CameraController.OnEvent(event);
}
