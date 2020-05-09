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
        ecs.AddComponent<Transform>(player, {{-2.0f, 4.0f, 0.9f}, {1.0f, 1.0f}, 0.0f});
        ecs.AddComponent<Colored>(player, {glm::vec4(1.0f)});
        ecs.AddComponent<Drawable>(player, {PrimitiveGeometryType::Quad});
        ecs.AddComponent<GDPlayer>(player, {GDGameMode::Cube, false});
        ecs.AddComponent<Gravity>(player, {{0.0f, -140.0f, 0.0f}});
        ecs.AddComponent<RigidBody>(player, {{15.0f, 0.0f, 0.0f}, glm::vec3(0.0f), 1.0f, true});
        ecs.AddComponent<GDObject>(player, {GDObjectType::Player});
        ecs.AddComponent<Textured>(player, {GDAssetManager::GetTexture("default-player.png"), 1.0f});
        return player;
    }

    inline Entity CreateSquare(ECS& ecs, const glm::vec3& position, unsigned groupID)
    {
        auto textureSquare = GDAssetManager::GetTexture("default-square.png");
        Entity square = ecs.CreateQuad(position, {1.0f, 1.0f}, textureSquare);
        ecs.RemoveComponent<Colored>(square);
        ecs.AddComponent<GDObject>(square, {GDObjectType::Square, false, true, groupID});
        return square;
    }

    inline Entity CreateTriangle(ECS& ecs, const glm::vec3& position, unsigned groupID)
    {
        auto textureTriangle = GDAssetManager::GetTexture("default-triangle.png");
        Entity triangle = ecs.CreateQuad(position, {1.0f, 1.0f}, textureTriangle);
        ecs.RemoveComponent<Colored>(triangle);
        ecs.AddComponent<GDObject>(triangle, {GDObjectType::Triangle, true, true, groupID});
        return triangle;
    }

    inline Entity CreateGround(ECS& ecs, const glm::vec3 position, unsigned groupID = 1)
    {
        auto textureGround = GDAssetManager::GetTexture("default-ground.png");
        Entity ground = ecs.CreateQuad(position, {5.0f, 10.0f}, glm::vec4(1.0f));
        ecs.RemoveComponent<Colored>(ground);
        ecs.AddComponent<GDObject>(ground, {GDObjectType::Ground, false, true, groupID});
        ecs.AddComponent<Textured>(ground, {textureGround, 1.0f});
        return ground;
    }

    inline Entity CreateBackground(ECS& ecs, const glm::vec3& position)
    {
        auto textureBackground = GDAssetManager::GetTexture("game_bg_01_001-uhd.png");
        Entity background = ecs.CreateQuad(position, {20.0f, 20.0f}, glm::vec4(1.0f));
        ecs.RemoveComponent<Colored>(background);
        ecs.AddComponent<Textured>(background, {textureBackground, 1.0f});
        ecs.AddComponent<GDObject>(background, {GDObjectType::Background, false, true, 0});
        return background;
    }

    inline Entity CreateMoveTrigger(ECS& ecs, const glm::vec3& position, unsigned groupID, float dt,
                                    const glm::vec3& ds)
    {
        Entity moveTrigger = ecs.CreateEntity();
        ecs.AddComponent<Transform>(moveTrigger, {position, {0.0f, 0.0f}, 0.0f});
        ecs.AddComponent<GDObject>(moveTrigger, {GDObjectType::Trigger, false, false});
        GDTrigger triggerProps = {GDTriggerType::Move, GDTriggerCycle::Ready, groupID, dt};
        triggerProps.MoveTriggerProps = {ds};
        static_assert(
            offsetof(decltype(triggerProps.MoveTriggerProps.DeltaPosition), x) ==
            offsetof(decltype(triggerProps.MoveTriggerProps), Dx)
        );
        ecs.AddComponent<GDTrigger>(moveTrigger, triggerProps);
        return moveTrigger;
    }

    inline Entity CreateColorTrigger(ECS& ecs, const glm::vec3& position, unsigned groupID, float dt,
                                     const glm::vec4& targetColor)
    {
        Entity colorTrigger = ecs.CreateEntity();
        ecs.AddComponent<Transform>(colorTrigger, {position, {0.0f, 0.0f}, 0.0f});
        ecs.AddComponent<GDObject>(colorTrigger, {GDObjectType::Trigger, false, false});
        GDTrigger triggerProps = {GDTriggerType::Color, GDTriggerCycle::Ready, groupID, dt};
        triggerProps.ColorTriggerProps = {targetColor, false};
        static_assert(
            offsetof(decltype(triggerProps.ColorTriggerProps.TargetColor), r) ==
            offsetof(decltype(triggerProps.ColorTriggerProps), R)
        );
        ecs.AddComponent<GDTrigger>(colorTrigger, triggerProps);
        return colorTrigger;
    }

    inline Entity CreateAccentColorTrigger(ECS& ecs, const glm::vec3& position, float dt, const glm::vec4& targetColor)
    {
        Entity colorTrigger = ecs.CreateEntity();
        ecs.AddComponent<Transform>(colorTrigger, {position, {0.0f, 0.0f}, 0.0f});
        ecs.AddComponent<GDObject>(colorTrigger, {GDObjectType::Trigger, false, false});
        GDTrigger triggerProps = {GDTriggerType::Color, GDTriggerCycle::Ready, 0, dt};
        triggerProps.ColorTriggerProps = {targetColor, true};
        static_assert(
            offsetof(decltype(triggerProps.ColorTriggerProps.TargetColor), r) ==
            offsetof(decltype(triggerProps.ColorTriggerProps), R)
        );
        ecs.AddComponent<GDTrigger>(colorTrigger, triggerProps);
        return colorTrigger;
    }
}
