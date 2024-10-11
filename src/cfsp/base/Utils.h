#pragma once

#include "mc/world/actor/player/Player.h"
#include <string>

namespace coral_fans::cfsp::utils {

std::pair<std::string, bool> getNbtFromTag(CompoundTag const, std::string const&);

std::string removeMinecraftPrefix(std::string const& s);

void swapItemInContainer(Player* player, int slot1, int slot2);

} // namespace coral_fans::cfsp::utils