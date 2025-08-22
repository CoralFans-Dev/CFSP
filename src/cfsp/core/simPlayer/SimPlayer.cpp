#include "SimPlayer.h"

namespace coral_fans::cfsp::simulated_player {
void save() {}

inline std::string SimPlayer::getName() { return this->name; }

inline std::string SimPlayer::getXuid() { return this->xuid; }

inline SimPlayer::SimPlayerStatus SimPlayer::getStatus() { return this->status; }
} // namespace coral_fans::cfsp::simulated_player