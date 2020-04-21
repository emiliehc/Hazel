#pragma once

#include "Displayable.h"
#include "Printable.h"

namespace Hazel
{
    class GameObject : public Printable
    {
    public:
        GameObject();

        virtual ~GameObject()
        {
        }

        std::string ToString() const override;

    private:
        std::string m_ID;
    };

    class DisplayableGameObject : public GameObject, public Displayable
    {
    public:
        DisplayableGameObject() : GameObject()
        {
        }

        ~DisplayableGameObject() override
        {
        }

        void Render() override
        {
        }
    };
}
