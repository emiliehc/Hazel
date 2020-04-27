#pragma once
#include "Hazel/ECS/ECSTypeDefs.h"

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
    };

    enum class GDObjectType : unsigned char {
        Player = 0,
        Ground = 1,
        Square = 2,
        Triangle = 3
    };

    struct GDObject
    {
        GDObjectType ObjectType;
        bool Killer = false;
    };
}
