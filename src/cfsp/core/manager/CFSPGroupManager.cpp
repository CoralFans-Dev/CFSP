#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include <unordered_set>
#include <vector>

namespace coral_fans::cfsp::manager {
#define IF_HAS_ARGS(...) , ##__VA_ARGS__

#define GROUP_FUNC_FROM_SP(GFUNC, SPFUNC, ...)                                                                         \
    std::vector<base::OperateResult> CFSPManager::group##GFUNC(                                                        \
        Player*                  player,                                                                               \
        std::string const& gname IF_HAS_ARGS(__VA_ARGS__)                                                              \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto checkResult = this->baseCheck(player, this->mPermissionConfig.group##GFUNC);                              \
        if (!checkResult) return {std::move(checkResult)};                                                             \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {base::OperateResult::error("manager.fail.groupNotExisted"_tr())};     \
        if (checkResult.mType != base::OperateResult::Type::Success                                                    \
            && !it->second->hasPermission(player, group::GroupPermission::GFUNC))                                      \
            return {base::OperateResult::error("manager.fail.permissionDenied"_tr())};                                 \
        std::vector<base::OperateResult> res;                                                                          \
        res.reserve(it->second->mData.splist.size());                                                                  \
        for (auto& spname : it->second->mData.splist) res.emplace_back(sp##SPFUNC);                                    \
        return res;                                                                                                    \
    }

base::OperateResult CFSPManager::groupCreate(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreateGroup(player); !checkResult) return checkResult;
    if (this->mGroupMap.find(gname) != this->mGroupMap.end())
        return base::OperateResult::error("manager.fail.groupHasExisted"_tr());
    // check: name
    if (!this->tryCreateDiretory(cfsp::CFSP::getInstance().getSelf().getDataDir() / "group", gname))
        return base::OperateResult::error("manager.fail.includeIllegalChar"_tr());
    group::GroupData groupData;
    groupData.name      = gname;
    groupData.ownerUuid = player ? player->getUuid().asString() : "";
    auto group          = std::make_shared<group::CFSPGroup>(groupData);
    group->save();
    this->mGroupMap.emplace(gname, group);

    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspGroup", {gname});
    return base::OperateResult::success("manager.success.create"_tr());
}

std::string CFSPManager::listGroup(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (this->isManager(player)) {
        res += "manager.info.groupCount"_tr(this->mGroupMap.size());
        for (auto group : this->mGroupMap) {
            res += "manager.info.group"_tr(group.first, base::utils::tryGetPlayerName(group.second->mData.ownerUuid));
            for (auto spname : group.second->mData.splist) res += "  " + spname + '\n';
        }
    } else {
        auto uuid  = player->getUuid().asString();
        int  count = 0;
        for (auto group : this->mGroupMap) {
            if (group.second->mData.ownerUuid != uuid) continue;
            count++;
            res += "manager.info.group"_tr(group.first, base::utils::tryGetPlayerName(group.second->mData.ownerUuid));
            for (auto spname : group.second->mData.splist) res += "  " + spname + '\n';
        }
        res = "manager.info.groupCount"_tr(count) + res;
    }
    return res;
}

base::OperateResult CFSPManager::groupDelete(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupDelete);
    if (!checkResult) return checkResult;
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return base::OperateResult::error("manager.fail.groupNotExisted"_tr());
    if (checkResult.mType != base::OperateResult::Type::Success
        && !it->second->hasPermission(player, group::GroupPermission::Delete))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspGroup", {gname});
    this->mGroupMap.erase(it);
    try {
        std::filesystem::remove_all(
            CFSP::getInstance().getSelf().getDataDir() / "group"
            / reinterpret_cast<const char8_t*>(it->second->mData.name.c_str())
        );
    } catch (...) {
        return base::OperateResult::error("manager.error.deleteFileFail"_tr());
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult CFSPManager::groupAddSp(Player* player, std::string const& gname, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupAddSp);
    if (!checkResult) return checkResult;
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return base::OperateResult::error("manager.fail.groupNotExisted"_tr());
    auto spIt = this->mOnlineSpMap.find(spname);
    if (spIt == this->mOnlineSpMap.end()) {
        spIt = this->mOfflineSpMap.find(spname);
        if (spIt == this->mOfflineSpMap.end()) return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (checkResult.mType != base::OperateResult::Type::Success
        && (!it->second->hasPermission(player, group::GroupPermission::AddSp)
            || !spIt->second->hasPermission(player, simulated_player::SimPlayerPermission::BeAddedToGroup)))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    it->second->mData.splist.emplace(spname);
    it->second->save();
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult CFSPManager::groupRmSp(Player* player, std::string const& gname, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupRmSp);
    if (!checkResult) return checkResult;
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return base::OperateResult::error("manager.fail.groupNotExisted"_tr());
    auto spIt = it->second->mData.splist.find(spname);
    if (spIt == it->second->mData.splist.end())
        return base::OperateResult::error("manager.fail.spNotExistedInGroup"_tr());
    if (checkResult.mType != base::OperateResult::Type::Success
        && !it->second->hasPermission(player, group::GroupPermission::AddSp))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    it->second->mData.splist.erase(spIt);
    it->second->save();
    return base::OperateResult::success("manager.success.operate"_tr());
}

std ::vector<base ::OperateResult> CFSPManager ::groupDeleteSp(Player* player, std ::string const& gname, bool force) {
    using ll ::i18n_literals ::operator""_tr;
    auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupDeleteSp);
    if (!checkResult) return {std ::move(checkResult)};
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return {base ::OperateResult ::error("manager.fail.groupNotExisted"_tr())};
    if (checkResult.mType != base ::OperateResult ::Type ::Success
        && !it->second->hasPermission(player, group ::GroupPermission ::DeleteSp))
        return {base ::OperateResult ::error("manager.fail.permissionDenied"_tr())};
    std::unordered_set<std::string> splist = {}, deleteFailList = {};
    std::swap(splist, it->second->mData.splist);
    std ::vector<base ::OperateResult> res;
    res.reserve(it->second->mData.splist.size());
    for (auto& spname : splist) {
        res.emplace_back(spDelete(player, spname, force, true));
        if (res.back().mType == base::OperateResult::Type::Swing) deleteFailList.emplace(spname);
    }
    std::swap(deleteFailList, it->second->mData.splist);
    it->second->save();
    return res;
}

std ::vector<base::OperateResult> CFSPManager::groupInfo(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (!isAllowed(player)) return {base::OperateResult::error("manager.fail.permissionDenied"_tr())};
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return {base::OperateResult::error("manager.fail.groupNotExisted"_tr())};
    if (!isManager(player) && !it->second->hasPermission(player, group::GroupPermission::None))
        return {base::OperateResult::error("manager.fail.permissionDenied"_tr())};
    std::vector<base::OperateResult> res;
    res.reserve(it->second->mData.splist.size());
    for (auto& spname : it->second->mData.splist) res.emplace_back(this->spInfo(player, spname, true));
    return res;
}

std ::vector<base::OperateResult> CFSPManager::groupInvInfo(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (!isAllowed(player)) return {base::OperateResult::error("manager.fail.permissionDenied"_tr())};
    auto it = this->mGroupMap.find(gname);
    if (it == this->mGroupMap.end()) return {base::OperateResult::error("manager.fail.groupNotExisted"_tr())};
    if (!isManager(player) && !it->second->hasPermission(player, group::GroupPermission::None))
        return {base::OperateResult::error("manager.fail.permissionDenied"_tr())};
    std::vector<base::OperateResult> res;
    res.reserve(it->second->mData.splist.size());
    for (auto& spname : it->second->mData.splist) res.emplace_back(spInvInfo(player, spname, true));
    return res;
}

GROUP_FUNC_FROM_SP(Spawn, Spawn(player, spname, true))
GROUP_FUNC_FROM_SP(Despawn, Despawn(player, spname, true))
GROUP_FUNC_FROM_SP(Respawn, Respawn(player, spname, true))
GROUP_FUNC_FROM_SP(Stop, Stop(player, spname, true))
GROUP_FUNC_FROM_SP(Drop, Drop(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(DropInv, DropInv(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(Sneaking, Sneaking(player, spname, enable, true), std::optional<bool> enable)
GROUP_FUNC_FROM_SP(Swimming, Swimming(player, spname, enable, true), std::optional<bool> enable)
GROUP_FUNC_FROM_SP(Flying, Flying(player, spname, enable, true), std::optional<bool> enable)
GROUP_FUNC_FROM_SP(Sprinting, Sprinting(player, spname, enable, true), std::optional<bool> enable)
GROUP_FUNC_FROM_SP(Attack, Attack(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(Build, Build(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(Interact, Interact(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(Jump, Jump(player, spname, times, interval, true), int times, int interval)
GROUP_FUNC_FROM_SP(Use, Use(player, spname, _long, times, interval, true), int _long, int times, int interval)
GROUP_FUNC_FROM_SP(Destroy, Destroy(player, spname, _long, times, interval, true), int _long, int times, int interval)
GROUP_FUNC_FROM_SP(Chat, Chat(player, spname, message, true), std::string const& message)
GROUP_FUNC_FROM_SP(RunCmd, RunCmd(player, spname, message, true), std::string const& message)
GROUP_FUNC_FROM_SP(LookAt, LookAt(player, spname, pos, true), Vec3 const& pos)
GROUP_FUNC_FROM_SP(LookAt, LookAt(player, spname, direction, true), simulated_player::SimPlayer::Direction direction)
GROUP_FUNC_FROM_SP(MoveTo, MoveTo(player, spname, pos, speed, true), Vec3 const& pos, float speed)
GROUP_FUNC_FROM_SP(NavTo, NavTo(player, spname, pos, speed, true), Vec3 const& pos, float speed)
GROUP_FUNC_FROM_SP(Tp, Tp(player, spname, pos, dimId, true), Vec3 pos, std::optional<int> dimId)
GROUP_FUNC_FROM_SP(Select, Select(player, spname, id, true), int id)
} // namespace coral_fans::cfsp::manager