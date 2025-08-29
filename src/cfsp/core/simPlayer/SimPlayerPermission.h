#pragma once

namespace coral_fans::cfsp::simulated_player {
enum class SimPlayerPermission : unsigned int {
    None    = 0,
    Online  = 1,
    Offline = 2,
};
}