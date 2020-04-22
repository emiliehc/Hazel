#pragma once

#include "Displayable2D.h"
#include "Printable.h"
#include "glm/glm.hpp"

namespace Hazel
{
    class GameObject : public Printable
    {
    public:
        GameObject(const glm::vec3& pos = glm::vec3(1.0f));

        virtual ~GameObject()
        {
        }

        std::string ToString() const override;

    private:
        std::string m_ID;
        glm::vec3 m_Pos;
    };

    class DisplayableGameObject2D : public GameObject, public Displayable2D
    {
    public:
        DisplayableGameObject2D(const glm::vec3& pos = glm::vec3(1.0f)) : GameObject(pos)
        {
        }

        ~DisplayableGameObject2D() override
        {
        }

        void Render() override
        {
        }
    };
}
