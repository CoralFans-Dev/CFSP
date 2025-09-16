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

uint CFSPManager::getGroupBasePermissionMask() {
    uint perm = 0;
    if (this->mPermissionConfig.groupDelete.enabled) perm |= (uint)group::GroupPermission::Delete;
    if (this->mPermissionConfig.groupAddSp.enabled) perm |= (uint)group::GroupPermission::AddSp;
    if (this->mPermissionConfig.groupRmSp.enabled) perm |= (uint)group::GroupPermission::RmSp;
    if (this->mPermissionConfig.groupSpawn.enabled) perm |= (uint)group::GroupPermission::Spawn;
    if (this->mPermissionConfig.groupDespawn.enabled) perm |= (uint)group::GroupPermission::Despawn;
    if (this->mPermissionConfig.groupRespawn.enabled) perm |= (uint)group::GroupPermission::Respawn;
    if (this->mPermissionConfig.groupDeleteSp.enabled) perm |= (uint)group::GroupPermission::DeleteSp;
    if (this->mPermissionConfig.groupStop.enabled) perm |= (uint)group::GroupPermission::Stop;
    if (this->mPermissionConfig.groupDrop.enabled) perm |= (uint)group::GroupPermission::Drop;
    if (this->mPermissionConfig.groupDropInv.enabled) perm |= (uint)group::GroupPermission::DropInv;
    if (this->mPermissionConfig.groupSneaking.enabled) perm |= (uint)group::GroupPermission::Sneaking;
    if (this->mPermissionConfig.groupSwimming.enabled) perm |= (uint)group::GroupPermission::Swimming;
    if (this->mPermissionConfig.groupFlying.enabled) perm |= (uint)group::GroupPermission::Flying;
    if (this->mPermissionConfig.groupSprinting.enabled) perm |= (uint)group::GroupPermission::Sprinting;
    if (this->mPermissionConfig.groupAttack.enabled) perm |= (uint)group::GroupPermission::Attack;
    if (this->mPermissionConfig.groupBuild.enabled) perm |= (uint)group::GroupPermission::Build;
    if (this->mPermissionConfig.groupInteract.enabled) perm |= (uint)group::GroupPermission::Interact;
    if (this->mPermissionConfig.groupJump.enabled) perm |= (uint)group::GroupPermission::Jump;
    if (this->mPermissionConfig.groupUse.enabled) perm |= (uint)group::GroupPermission::Use;
    if (this->mPermissionConfig.groupDestroy.enabled) perm |= (uint)group::GroupPermission::Destroy;
    if (this->mPermissionConfig.groupChat.enabled) perm |= (uint)group::GroupPermission::Chat;
    if (this->mPermissionConfig.groupRunCmd.enabled) perm |= (uint)group::GroupPermission::RunCmd;
    if (this->mPermissionConfig.groupLookAt.enabled) perm |= (uint)group::GroupPermission::LookAt;
    if (this->mPermissionConfig.groupMoveTo.enabled) perm |= (uint)group::GroupPermission::MoveTo;
    if (this->mPermissionConfig.groupNavTo.enabled) perm |= (uint)group::GroupPermission::NavTo;
    if (this->mPermissionConfig.groupTp.enabled) perm |= (uint)group::GroupPermission::Tp;
    if (this->mPermissionConfig.groupSelect.enabled) perm |= (uint)group::GroupPermission::Select;
    return perm;
}

uint CFSPManager::getGroupPermissionMask(CommandPermissionLevel level) {
    uint perm = 0;
    if (level >= this->mPermissionConfig.groupDelete.permission) perm |= (uint)group::GroupPermission::Delete;
    if (level >= this->mPermissionConfig.groupAddSp.permission) perm |= (uint)group::GroupPermission::AddSp;
    if (level >= this->mPermissionConfig.groupRmSp.permission) perm |= (uint)group::GroupPermission::RmSp;
    if (level >= this->mPermissionConfig.groupSpawn.permission) perm |= (uint)group::GroupPermission::Spawn;
    if (level >= this->mPermissionConfig.groupDespawn.permission) perm |= (uint)group::GroupPermission::Despawn;
    if (level >= this->mPermissionConfig.groupRespawn.permission) perm |= (uint)group::GroupPermission::Respawn;
    if (level >= this->mPermissionConfig.groupDeleteSp.permission) perm |= (uint)group::GroupPermission::DeleteSp;
    if (level >= this->mPermissionConfig.groupStop.permission) perm |= (uint)group::GroupPermission::Stop;
    if (level >= this->mPermissionConfig.groupDrop.permission) perm |= (uint)group::GroupPermission::Drop;
    if (level >= this->mPermissionConfig.groupDropInv.permission) perm |= (uint)group::GroupPermission::DropInv;
    if (level >= this->mPermissionConfig.groupSneaking.permission) perm |= (uint)group::GroupPermission::Sneaking;
    if (level >= this->mPermissionConfig.groupSwimming.permission) perm |= (uint)group::GroupPermission::Swimming;
    if (level >= this->mPermissionConfig.groupFlying.permission) perm |= (uint)group::GroupPermission::Flying;
    if (level >= this->mPermissionConfig.groupSprinting.permission) perm |= (uint)group::GroupPermission::Sprinting;
    if (level >= this->mPermissionConfig.groupAttack.permission) perm |= (uint)group::GroupPermission::Attack;
    if (level >= this->mPermissionConfig.groupBuild.permission) perm |= (uint)group::GroupPermission::Build;
    if (level >= this->mPermissionConfig.groupInteract.permission) perm |= (uint)group::GroupPermission::Interact;
    if (level >= this->mPermissionConfig.groupJump.permission) perm |= (uint)group::GroupPermission::Jump;
    if (level >= this->mPermissionConfig.groupUse.permission) perm |= (uint)group::GroupPermission::Use;
    if (level >= this->mPermissionConfig.groupDestroy.permission) perm |= (uint)group::GroupPermission::Destroy;
    if (level >= this->mPermissionConfig.groupChat.permission) perm |= (uint)group::GroupPermission::Chat;
    if (level >= this->mPermissionConfig.groupRunCmd.permission) perm |= (uint)group::GroupPermission::RunCmd;
    if (level >= this->mPermissionConfig.groupLookAt.permission) perm |= (uint)group::GroupPermission::LookAt;
    if (level >= this->mPermissionConfig.groupMoveTo.permission) perm |= (uint)group::GroupPermission::MoveTo;
    if (level >= this->mPermissionConfig.groupNavTo.permission) perm |= (uint)group::GroupPermission::NavTo;
    if (level >= this->mPermissionConfig.groupTp.permission) perm |= (uint)group::GroupPermission::Tp;
    if (level >= this->mPermissionConfig.groupSelect.permission) perm |= (uint)group::GroupPermission::Select;
    return perm;
}

uint CFSPManager::getGroupPermissionMask(std::optional<CommandPermissionLevel> level) {
    static uint baseMask = this->getGroupBasePermissionMask();
    if (!level.has_value()) return baseMask;
    switch (level.value()) {
    case CommandPermissionLevel::Any:
        static uint maskAny = baseMask & getGroupPermissionMask(level.value());
        return maskAny;
    case CommandPermissionLevel::GameDirectors:
        static uint maskGameDirectors = baseMask & getGroupPermissionMask(level.value());
        return maskGameDirectors;
    case CommandPermissionLevel::Admin:
        static uint maskAdmin = baseMask & getGroupPermissionMask(level.value());
        return maskAdmin;
    case CommandPermissionLevel::Host:
        static uint maskHost = baseMask & getGroupPermissionMask(level.value());
        return maskHost;
    case CommandPermissionLevel::Owner:
        static uint maskOwner = baseMask & getGroupPermissionMask(level.value());
        return maskOwner;
    case CommandPermissionLevel::Internal:
        static uint maskInternal = baseMask & getGroupPermissionMask(level.value());
        return maskInternal;
    }
    return 0;
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
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspGroup", {gname});
    std::filesystem::remove_all(
        CFSP::getInstance().getSelf().getDataDir() / "group"
        / reinterpret_cast<const char8_t*>(it->second->mData.name.c_str())
    );
    this->mGroupMap.erase(it);
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
    for (auto& spname : it->second->mData.splist) res.emplace_back(spInfo(player, spname, true));
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
GROUP_FUNC_FROM_SP(Drop, Drop(player, spname, true))
GROUP_FUNC_FROM_SP(DropInv, DropInv(player, spname, true))
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
GROUP_FUNC_FROM_SP(MoveTo, MoveTo(player, spname, pos, speed, true), Vec3 const& pos, float speed)
GROUP_FUNC_FROM_SP(NavTo, NavTo(player, spname, pos, speed, true), Vec3 const& pos, float speed)
GROUP_FUNC_FROM_SP(Tp, Tp(player, spname, pos, dimId, true), Vec3 pos, std::optional<int> dimId)
GROUP_FUNC_FROM_SP(Select, Select(player, spname, id, true), int id)
} // namespace coral_fans::cfsp::manager