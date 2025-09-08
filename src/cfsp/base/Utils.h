#pragma once

#include "mc/world/level/BlockPos.h"
#include <string>

namespace coral_fans::cfsp::base::utils {

std::string tryGetPlayerName(std::string UUID);

BlockPos neighbor(BlockPos pos, int face);

std::string getDimName(int dimId);

std::string getGameModeStr(int gameType);
} // namespace coral_fans::cfsp::base::utils