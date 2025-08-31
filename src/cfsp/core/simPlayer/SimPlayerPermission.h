#pragma once

namespace coral_fans::cfsp::simulated_player {
enum class SimPlayerPermission : unsigned int {
    None    = 0,
    Spawn   = 1 << 0,
    Despawn = 1 << 1,
    Respawn = 1 << 2,
};
}