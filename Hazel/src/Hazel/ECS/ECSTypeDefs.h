#pragma once
#include <bitset>

namespace Hazel
{
    using Entity = unsigned;
    constexpr unsigned MAX_ENTITIES = 5000;

    using ComponentType = unsigned char;
    const unsigned char MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

}
