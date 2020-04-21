#pragma once

namespace Hazel
{
    class Printable
    {
    public:
        virtual ~Printable() = default;

        virtual std::string ToString() const = 0;

        friend std::ostream& operator<<(std::ostream& os, const Printable& printable);
    };
}
