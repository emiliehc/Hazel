#pragma once
#include "glm/glm.hpp"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
    struct Gravity
    {
        glm::vec3 Force;
    };

    struct Transform
    {
        glm::vec3 Position;
        glm::vec2 Size;
        float Rotation;
    };

    struct Colored
    {
        glm::vec4 Color;
    };

    struct Textured
    {
        Ref<Texture2D> Texture;
        float TilingFactor;
    };

    // extend this to support n-gons, which are basically Ref<Mesh>es
    enum class PrimitiveGeometryType : unsigned char
    {
        Triangle = 0, Quad = 1
    };

    struct Drawable
    {
        PrimitiveGeometryType GeometryType;
    };

    struct RigidBody
    {
        glm::vec3 Velocity;
        glm::vec3 Acceleration;
        bool Movable;
    };

    struct Health
    {
        float health = 1.0f;
    };
}
