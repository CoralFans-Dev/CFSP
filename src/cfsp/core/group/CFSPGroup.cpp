#include "CFSPGroup.h"
#include "cfsp/CFSP.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/Config.h"

namespace coral_fans::cfsp::group {
CFSPGroup::CFSPGroup(GroupData groupData) { this->mData = groupData; }

void CFSPGroup::save() {
    ll::config::saveConfig(
        this->mData,
        CFSP::getInstance().getSelf().getDataDir() / "group"
            / reinterpret_cast<const char8_t*>(this->mData.name.c_str()) / "data.json"
    );
}

bool CFSPGroup::hasPermission(Player* player, GroupPermission permission) {
    if (manager::CFSPManager::getInstance().isManager(player)) return true;
    auto uuid = player->getUuid().asString();
    if (this->mData.ownerUuid == uuid) return true;
    auto it = this->mData.permission.find(uuid);
    if (it != this->mData.permission.end() && (it->second & (uint)permission) == (uint)permission) return true;
    return (this->mData.publicPermission & (uint)permission) == (uint)permission;
}

uint CFSPGroup::getPermission(const Player* player) {
    auto uuid = player->getUuid().asString();
    if (this->mData.ownerUuid == uuid) return (uint)-1;
    if (auto it = this->mData.permission.find(uuid); it != this->mData.permission.end())
        return (it->second | this->mData.publicPermission);
    return this->mData.publicPermission;
}
} // namespace coral_fans::cfsp::group