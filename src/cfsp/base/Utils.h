#pragma once

#include "mc/world/actor/player/Player.h"
#include <string>

namespace coral_fans::cfsp::utils {

std::pair<std::string, bool> getNbtFromTag(CompoundTag const, std::string const&);

std::string tryGetPlayerName(std::string UUID);

BlockPos neighbor(BlockPos pos, int face);

} // namespace coral_fans::cfsp::utils