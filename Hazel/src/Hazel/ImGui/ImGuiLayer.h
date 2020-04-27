#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Layer.h"

namespace Hazel {
    class HAZEL_API ImGuiLayer : public Hazel::Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;

        void Begin();
        void End();
    private:
        float m_Time = 0.0f;
    };
}