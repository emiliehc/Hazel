#include "Hazel.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>


class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6, 1.6, -0.9, 0.9), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
    {
        m_VertexArray.reset(Hazel::VertexArray::Create());


        float vertices[3 * 7] = {
            -0.5, -0.5, 0, 0.8, 0.2, 0.8, 1,
            0.5, -0.5, 0, 0.2, 0.3, 0.8, 1,
            0, 0.5, 0, 0.8, 0.8, 0.2, 1
        };

        std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));


        Hazel::BufferLayout layout = {
            {Hazel::ShaderDataType::Float3, "a_Position"},
            {Hazel::ShaderDataType::Float4, "a_Color"}
        };

        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);


        unsigned int indices[3] = {0, 1, 2};
        std::shared_ptr<Hazel::IndexBuffer> indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
        m_VertexArray->SetIndexBuffer(indexBuffer);


        float squareVertices[3 * 4] = {
            -0.5, -0.5, 0,
            0.5, -0.5, 0,
            0.5, 0.5, 0,
            -0.5, 0.5, 0
        };


        m_SquareVA.reset(Hazel::VertexArray::Create());
        std::shared_ptr<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            {Hazel::ShaderDataType::Float3, "a_Position"}
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = {
            0, 1, 2,
            2, 3, 0
        };
        std::shared_ptr<Hazel::IndexBuffer> squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareVertices) / sizeof(unsigned int)));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            //gl_Position = vec4(a_Position, 1.0);
            v_Position = a_Position;
            v_Color = a_Color;
        }

        )";

        std::string fragmentSrc =
            R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main() {
            color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
            
        )";

        std::string flatColorShaderVertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;

        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            v_Position = a_Position;
        }

        )";

        std::string flatColorShaderFragmentSrc =
            R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        uniform vec4 u_Color;

        in vec3 v_Position;

        void main() {
            color = u_Color;
        }
            
        )";

        m_Shader = std::make_unique<Hazel::Shader>(vertexSrc, fragmentSrc);
        m_FlatColorShader = std::make_unique<Hazel::Shader>(flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
    }

    void OnUpdate(Hazel::Timestep ts) override
    {
        //HZ_TRACE("{0}", ts);
        if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
        {
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        }
        if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
        {
            m_CameraPosition.x += m_CameraMoveSpeed * ts;
        }
        if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
        {
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        }
        if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
        {
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        }


        if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraRotation += m_CameraRotationSpeed * ts;
        }
        if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraRotation -= m_CameraRotationSpeed * ts;
        }

        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();

        m_Camera.SetRotation(m_CameraRotation);
        m_Camera.SetPosition(m_CameraPosition);


        Hazel::Renderer::BeginScene(m_Camera);

        static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
        glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);


        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++) {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = translate(glm::mat4(1.0f), pos) * scale;
                m_FlatColorShader->UploadUniformFloat4("u_Color", x % 2 == 0 ? redColor : blueColor);
                Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }
        
        Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
    }

    void OnImGuiRender() override
    {
    }

    void OnEvent(Hazel::Event& event) override
    {
    }

private:

    std::shared_ptr<Hazel::Shader> m_Shader;
    std::shared_ptr<Hazel::Shader> m_FlatColorShader;
    std::shared_ptr<Hazel::VertexArray> m_VertexArray;
    std::shared_ptr<Hazel::VertexArray> m_SquareVA;

    Hazel::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 1;
    float m_CameraRotationSpeed = 45;
    float m_CameraRotation = 0.0f;

    glm::vec3 m_SquarePosition;
};


class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer);
    }

    ~Sandbox() override = default;
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox;
}
