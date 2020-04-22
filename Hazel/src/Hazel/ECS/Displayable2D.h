#pragma once

namespace Hazel
{
    class Displayable2D
    {
    public:
        virtual ~Displayable2D() = default;
        virtual void Render() = 0;
    };
}
