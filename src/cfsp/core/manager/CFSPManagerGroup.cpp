#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"


namespace coral_fans::cfsp::manager {
base::OperateResult CFSPManager::createGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreateGroup(player)) return checkResult;
    if (this->mGroupMap.find(gname) != this->mGroupMap.end())
        return base::OperateResult::error("manager.fail.groupHasExisted"_tr());

    group::GroupData groupData;
    groupData.name      = gname;
    groupData.ownerUuid = player->getUuid().asString();
    this->mGroupMap.emplace(gname, std::make_shared<group::CFSPGroup>(groupData));

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspGroup", {gname});
    return base::OperateResult::success("manager.success.create"_tr());
}

std::string CFSPManager::listGroup(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (this->isManager(player)) {
        for (auto group : this->mGroupMap) {
            res += "manager.info.group"_tr(group.first, base::utils::tryGetPlayerName(group.second->mData.ownerUuid));
            for (auto spname : group.second->mData.splist) res += "  " + spname + '\n';
        }
    } else {
        auto uuid = player->getUuid().asString();
        for (auto group : this->mGroupMap) {
            if (group.second->mData.ownerUuid != uuid) continue;
            res += "manager.info.group"_tr(group.first, base::utils::tryGetPlayerName(group.second->mData.ownerUuid));
            for (auto spname : group.second->mData.splist) res += "  " + spname + '\n';
        }
    }
    return res;
}
} // namespace coral_fans::cfsp::manager