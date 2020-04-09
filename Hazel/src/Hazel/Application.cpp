#include "hzpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "glad/glad.h"
#include "Input.h"
#include "Log.h"

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    static unsigned int ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case Hazel::ShaderDataType::Float:
        case Hazel::ShaderDataType::Float2:
        case Hazel::ShaderDataType::Float3:
        case Hazel::ShaderDataType::Float4:
        case Hazel::ShaderDataType::Mat3:
        case Hazel::ShaderDataType::Mat4:
            return GL_FLOAT;
        case Hazel::ShaderDataType::Int:
        case Hazel::ShaderDataType::Int2:
        case Hazel::ShaderDataType::Int3:
        case Hazel::ShaderDataType::Int4:
            return GL_INT;
        case Hazel::ShaderDataType::Bool:
            return GL_BOOL;
        default:
            break;
        }

        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    Application::Application()
    {
        HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);


        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);

        float vertices[3 * 7] = {
            -0.5, -0.5, 0, 0.8, 0.2, 0.8, 1,
            0.5, -0.5, 0, 0.2, 0.3, 0.8, 1,
            0, 0.5, 0, 0.8, 0.8, 0.2, 1
        };


        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

        {
            BufferLayout layout = {
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"}
            };

            m_VertexBuffer->SetLayout(layout);
        }

        unsigned int index = 0;
        const auto& layout = m_VertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                m_VertexBuffer->GetLayout().GetStride(),
                (const void*)element.Offset);
            index++;
        }

        unsigned int indices[3] = {0, 1, 2};
        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));


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

        m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
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

            m_Shader->Bind();
            glBindVertexArray(m_VertexArray);
            glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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
