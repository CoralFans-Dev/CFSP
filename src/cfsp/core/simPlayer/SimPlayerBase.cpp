#include "SimPlayer.h"
#include <boost/iostreams/device/mapped_file.hpp>

namespace coral_fans::cfsp::simulated_player {
inline std::string SimPlayer::getName() { return this->mSaveData.name; }

inline std::string SimPlayer::getXuid() { return this->mSaveData.xuid; }

inline bool SimPlayer::isOnline() { return this->mSimPlayer != nullptr; }
} // namespace coral_fans::cfsp::simulated_player