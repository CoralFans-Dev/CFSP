#pragma once

namespace coral_fans::cfsp::group {
enum class GroupPermission : unsigned int {
    None      = 0,
    AddSp     = 1 << 0,
    RmSp      = 1 << 1,
    Delete    = 1 << 2,
    Spawn     = 1 << 3,
    Despawn   = 1 << 4,
    Respawn   = 1 << 5,
    DeleteSp  = 1 << 6,
    Stop      = 1 << 7,
    Drop      = 1 << 8,
    DropInv   = 1 << 9,
    Sneaking  = 1 << 10,
    Swimming  = 1 << 11,
    Flying    = 1 << 12,
    Sprinting = 1 << 13,
    Attack    = 1 << 14,
    Build     = 1 << 15,
    Interact  = 1 << 16,
    Jump      = 1 << 17,
    Use       = 1 << 18,
    Destroy   = 1 << 19,
    Chat      = 1 << 20,
    RunCmd    = 1 << 21,
    LookAt    = 1 << 22,
    MoveTo    = 1 << 23,
    NavTo     = 1 << 24,
    Tp        = 1 << 25,
    Select    = 1 << 26,
};
}