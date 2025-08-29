#pragma once

#include "GroupData.h"
#include "mc/world/actor/player/Player.h"

namespace coral_fans::cfsp::group {
class CFSPGroup {
public:
    GroupData mData;

public:
    bool hasPermission(Player*, GroupPermission permission);
};
} // namespace coral_fans::cfsp::group