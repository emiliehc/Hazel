#pragma once

namespace GD
{
    // Additional GD specific components e.g. move trigger, color trigger

    // trigger (implement in the game logic system)
    // When the player collides with the trigger, the trigger performs the actions specified
    // by its type, e.g. the move trigger will move the blocks specified by their entity ids
    // by the number of blocks specified in the struct. The time step will be used together
    // with the action time to calculate the amount of movements needed for each frame.
}
