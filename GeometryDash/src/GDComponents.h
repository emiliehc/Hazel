#pragma once
#include "Hazel/ECS/ECSTypeDefs.h"
#include <glm/glm.hpp>

using namespace Hazel;

namespace GD
{
    // Additional GD specific components e.g. move trigger, color trigger

    // trigger (implement in the game logic system)
    // When the player collides with the trigger, the trigger performs the actions specified
    // by its type, e.g. the move trigger will move the blocks specified by their entity ids
    // by the number of blocks specified in the struct. The time step will be used together
    // with the action time to calculate the amount of movements needed for each frame.

    enum class GDGameMode : unsigned char
    {
        Cube = 0
    };

    struct GDPlayer
    {
        GDGameMode GameMode;
        bool OnTheGround;
        bool Alive = true;
    };

    enum class GDObjectType : unsigned char
    {
        Player = 0,
        Ground = 1,
        Square = 2,
        Triangle = 3,
        Background = 4,
        Trigger = 5,
        JumpRing = 6
    };

    struct GDObject
    {
        GDObjectType ObjectType;
        bool Killer = false;
        bool Visible = true;
        /**
         * Not being marked as const does not mean you should change it
         */
        unsigned GroupID = 0;
        /**
         * Default IDs:
         * Background = 0,
         * Ground = 1
         */
    };

    enum class GDTriggerType : unsigned char
    {
        Invalid = 0,
        Move = 1,
        Color = 2
    };

    enum class GDTriggerCycle : unsigned char
    {
        Invalid = 0,
        Ready = 1,
        Triggered = 2
    };

    struct GDTrigger
    {
        GDTriggerType Type;
        GDTriggerCycle Cycle;
        unsigned int ObjectGroupID; // what objects does this trigger affect?
        float Duration;
        float ElapsedTime = 0.0f;

        GDTrigger() : Type(GDTriggerType::Invalid), Cycle(GDTriggerCycle::Invalid), ObjectGroupID(0), Duration(0.0f)
        {
        }

        GDTrigger(GDTriggerType type, GDTriggerCycle cycle, unsigned groupID, float duration) : Type(type),
                                                                                                Cycle(cycle),
                                                                                                ObjectGroupID(groupID),
                                                                                                Duration(duration)
        {
        }

        union
        {
            struct GDMoveTriggerProps
            {
                union
                {
                    glm::vec3 DeltaPosition;

                    struct
                    {
                        float Dx;
                        float Dy;
                        float Dz;
                    };
                };
            } MoveTriggerProps;

            struct GDColorTriggerProps
            {
                union
                {
                    glm::vec4 TargetColor;

                    struct
                    {
                        float R;
                        float G;
                        float B;
                        float A;
                    };
                };

                bool AffectsAccentColor = false;
            } ColorTriggerProps;
        };
    };

    enum class GDJumpRingType : unsigned char
    {
        Yellow = 0,
        Pink = 1,
        Blue = 2,
        Red = 3,
        Green = 4
    };

    struct GDJumpRingProps
    {
        GDJumpRingType Type;
    };
}
