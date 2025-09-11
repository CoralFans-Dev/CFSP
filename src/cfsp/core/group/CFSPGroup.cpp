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
            / ("-" + std::to_string(std::hash<std::string>()(this->mData.name))) / "data.json"
    );
}

bool CFSPGroup::hasPermission(Player* player, GroupPermission permission) {
    if (manager::CFSPManager::getInstance().isManager(player)) return true;
    auto uuid = player->getUuid().asString();
    if (this->mData.ownerUuid == uuid) return true;
    auto it = this->mData.permission.find(uuid);
    if (it != this->mData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission) return true;
    it = this->mData.permission.find("");
    return it != this->mData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission;
}
} // namespace coral_fans::cfsp::group