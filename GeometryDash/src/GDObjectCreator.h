#pragma once

#include "GDAssetManager.h"
#include "GDComponents.h"
#include "glm/vec3.hpp"
#include "Hazel/ECS/Components.h"
#include "Hazel/ECS/ECSCore.h"
#include "Hazel/ECS/ECSTypeDefs.h"

namespace GD
{
    inline Entity CreatePlayer(ECS& ecs)
    {
        Entity player = ecs.CreateEntity();
        ecs.AddComponent<Transform>(player, {{0.0f, 100.0f, 0.9f}, {1.0f, 1.0f}, 0.0f});
        ecs.AddComponent<Colored>(player, {glm::vec4(1.0f)});
        ecs.AddComponent<Drawable>(player, {PrimitiveGeometryType::Quad});
        ecs.AddComponent<GDPlayer>(player, {GDGameMode::Cube, false});
        ecs.AddComponent<Gravity>(player, {{0.0f, -140.0f, 0.0f}});
        ecs.AddComponent<RigidBody>(player, {glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, true});
        ecs.AddComponent<GDObject>(player, {GDObjectType::Player});
        ecs.AddComponent<Textured>(player, {GDAssetManager::GetTexture("default-player.png"), 1.0f});
        return player;
    }

    inline Entity CreateSquare(ECS& ecs, const glm::vec3& position)
    {
        auto textureSquare = GDAssetManager::GetTexture("default-square.png");
        Entity square = ecs.CreateQuad(position, {1.0f, 1.0f}, textureSquare);
        ecs.RemoveComponent<Colored>(square);
        ecs.AddComponent<GDObject>(square, {GDObjectType::Square, false});
        return square;
    }

    inline Entity CreateTriangle(ECS& ecs, const glm::vec3& position)
    {
        auto textureTriangle = GDAssetManager::GetTexture("default-triangle.png");
        Entity triangle = ecs.CreateQuad(position, {1.0f, 1.0f}, textureTriangle);
        ecs.RemoveComponent<Colored>(triangle);
        ecs.AddComponent<GDObject>(triangle, {GDObjectType::Triangle, true});
        return triangle;
    }
}
