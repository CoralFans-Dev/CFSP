#pragma once

namespace coral_fans::cfsp::simulated_player {
enum class SimPlayerPermission : unsigned int {
    None           = 0,
    Spawn          = 1 << 0,
    Despawn        = 1 << 1,
    Respawn        = 1 << 2,
    Delete         = 1 << 3,
    Stop           = 1 << 4,
    Drop           = 1 << 5,
    DropInv        = 1 << 6,
    Swap           = 1 << 7,
    Sneaking       = 1 << 8,
    Swimming       = 1 << 9,
    Flying         = 1 << 10,
    Sprinting      = 1 << 11,
    Attack         = 1 << 12,
    Build          = 1 << 13,
    Interact       = 1 << 14,
    Jump           = 1 << 15,
    Use            = 1 << 16,
    Destroy        = 1 << 17,
    Chat           = 1 << 18,
    RunCmd         = 1 << 19,
    Select         = 1 << 20,
    LookAt         = 1 << 21,
    MoveTo         = 1 << 22,
    NavTo          = 1 << 23,
    Tp             = 1 << 24,
    BeAddedToGroup = 1 << 25,
};
}