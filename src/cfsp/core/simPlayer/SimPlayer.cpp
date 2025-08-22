#include "SimPlayer.h"

namespace coral_fans::cfsp::simulated_player {
inline std::string SimPlayer::getName() { return this->name; }
inline std::string SimPlayer::getXuid() { return this->xuid; }
inline int         SimPlayer::getStatus() { return this->status; }
} // namespace coral_fans::cfsp::simulated_player