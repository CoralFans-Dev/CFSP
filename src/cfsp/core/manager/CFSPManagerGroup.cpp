#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
base::OperateResult CFSPManager::groupCreate(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreateGroup(player)) return checkResult;
    if (this->mGroupMap.find(gname) != this->mGroupMap.end())
        return base::OperateResult::error("manager.fail.groupHasExisted"_tr());

    group::GroupData groupData;
    groupData.name      = gname;
    groupData.ownerUuid = player->getUuid().asString();
    auto group          = std::make_shared<group::CFSPGroup>(groupData);
    group->save();
    this->mGroupMap.emplace(gname, group);

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

base::OperateResult CFSPManager::groupRm(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupRm);
    if (!checkResult) return checkResult;
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return base::OperateResult::error("manager.fail.groupNotExisted"_tr());
    if (checkResult.mType != base::OperateResult::Type::Success
        && !it->second->hasPermission(player, group::GroupPermission::Rm))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspGroup", {gname});
    std::filesystem::remove_all(
        CFSP::getInstance().getSelf().getDataDir() / "group"
        / ("-" + std::to_string(std::hash<std::string>()(it->second->mData.name)))
    );
    this->mGroupMap.erase(it);
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::manager