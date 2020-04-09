#include "hzpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "glad/glad.h"
#include "Log.h"

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);
        
        m_VertexArray.reset(VertexArray::Create());

        
        float vertices[3 * 7] = {
            -0.5, -0.5, 0, 0.8, 0.2, 0.8, 1,
            0.5, -0.5, 0, 0.2, 0.3, 0.8, 1,
            0, 0.5, 0, 0.8, 0.8, 0.2, 1
        };

        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));


        BufferLayout layout = {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"}
        };

        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);


        unsigned int indices[3] = {0, 1, 2};
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        

        float squareVertices[3 * 4] = {
            -0.75, -0.75, 0,
            0.75, -0.75, 0,
            0.75, 0.75, 0,
            -0.75, 0.75, 0
        };


        m_SquareVA.reset(VertexArray::Create());
        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            {ShaderDataType::Float3, "a_Position"}
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = {
            0, 1, 2,
            2, 3, 0
        };
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareVertices) / sizeof(unsigned int)));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            gl_Position = vec4(a_Position, 1.0);
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

        std::string blueShaderVertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;

        out vec3 v_Position;

        void main() {
            gl_Position = vec4(a_Position, 1.0);
            v_Position = a_Position;
        }

        )";

        std::string blueShaderFragmentSrc =
            R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        void main() {
            color = vec4(0.2, 0.3, 0.8, 1.0);
        }
            
        )";

        m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
        m_BlueShader = std::make_unique<Shader>(blueShaderVertexSrc, blueShaderFragmentSrc);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
        //HZ_CORE_TRACE("{0}", e);

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
            {
                break;
            }
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    Application::~Application() = default;

    void Application::Run()
    {
        while (m_Running)
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_BlueShader->Bind();
            m_SquareVA->Bind();
            glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            

            m_Shader->Bind();
            m_VertexArray->Bind();

            glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
            

            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();


            //auto [x, y] = Input::GetMousePosition();
            //HZ_CORE_TRACE("{0}, {1}", x, y);

            m_Window->OnUpdate();
        }
    }
}
