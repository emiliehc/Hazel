#include "hzpch.h"
#include "Printable.h"

namespace Hazel
{
    std::ostream& operator<<(std::ostream& os, const Printable& printable) {
        os << printable.ToString();
        return os;
    }
}
