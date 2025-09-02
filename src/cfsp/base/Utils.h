#pragma once

#include "mc/world/level/BlockPos.h"
#include <string>

namespace coral_fans::cfsp::utils {

std::string tryGetPlayerName(std::string UUID);

BlockPos neighbor(BlockPos pos, int face);

} // namespace coral_fans::cfsp::utils