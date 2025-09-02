#pragma once

namespace coral_fans::cfsp::simulated_player {
enum class SimPlayerPermission : unsigned int {
    None    = 0,
    Spawn   = 1 << 0,
    Despawn = 1 << 1,
    Respawn = 1 << 2,
    Rm      = 1 << 3,
    Stop    = 1 << 4,
    Drop    = 1 << 5,
    DropInv = 1 << 6,
    Swap    = 1 << 7,
};
}