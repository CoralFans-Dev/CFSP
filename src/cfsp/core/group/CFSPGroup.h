#pragma once

#include "GroupData.h"
#include "mc/world/actor/player/Player.h"

namespace coral_fans::cfsp::group {
class CFSPGroup {
public:
    GroupData mData;

public:
    CFSPGroup(GroupData groupData);

public:
    void save();
    bool hasPermission(Player*, GroupPermission permission);
    uint getPermission(const Player* player);
};
} // namespace coral_fans::cfsp::group