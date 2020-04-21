#pragma once

namespace Hazel
{
    class Displayable
    {
    public:
        virtual ~Displayable() = default;
        virtual void Render() = 0;
    };
}
