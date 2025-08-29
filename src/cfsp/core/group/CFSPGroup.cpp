#include "CFSPGroup.h"
#include "cfsp/core/group/CFSPGroup.h"

namespace coral_fans::cfsp::group {
bool CFSPGroup::hasPermission(Player* player, GroupPermission permission) {
    auto uuid = player->getUuid().asString();
    if (this->mData.ownerUuid == uuid) return true;
    auto it = this->mData.permission.find(uuid);
    if (it != this->mData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission) return true;
    it = this->mData.permission.find("");
    return it != this->mData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission;
}
} // namespace coral_fans::cfsp::group