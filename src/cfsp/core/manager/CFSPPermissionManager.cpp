#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/i18n/I18n.h"
#include <optional>

namespace coral_fans::cfsp::manager {
bool CFSPManager::isAllowed(const Player* player) {
    if (!player) return true;
    if (player->isSimulatedPlayer()) [[unlikely]]
        return false;
    switch (this->mConfig.listType) {
    case coral_fans::cfsp::config::ListType::disabled:
        return true;
    case coral_fans::cfsp::config::ListType::blacklist:
        if (this->mConfig.list.find(player->mName) != this->mConfig.list.end()) return false;
        return true;
    case coral_fans::cfsp::config::ListType::whitelist:
        if (this->mConfig.list.find(player->mName) == this->mConfig.list.end()) return false;
        return true;
    }
    return true;
}

bool CFSPManager::isManager(const Player* player) {
    return !player // 当player==null时，为控制台在执行命令
        || player->getCommandPermissionLevel() >= this->mConfig.adminPermission
        || this->mConfig.superManagerList.contains(player->getUuid().asString());
}

base::OperateResult CFSPManager::baseCheck(const Player* player, config::FuncStruct func) {
    using ll::i18n_literals::operator""_tr;
    if (!func.enabled) [[unlikely]]
        return base::OperateResult::error("manager.fail.funcUnabled"_tr());
    if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (this->isManager(player)) return base::OperateResult::success();
    if (player->getCommandPermissionLevel() < func.permission)
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return base::OperateResult();
}

base::OperateResult CFSPManager::canCreatePlayer(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spCreate);
        checkResult.mType != base::OperateResult::Type::Swing)
        return checkResult;
    auto uuid = player->getUuid().asString();
    // check: maxOnline
    if (this->mOnlineSpMap.size() >= this->mConfig.maxOnline)
        return base::OperateResult::error("manager.fail.tooManyOnline"_tr(std::to_string(this->mConfig.maxOnline)));
    // check: maxOnlinePerPlayer
    unsigned long long spawnCount = 0, OwnCount = 0;
    for (auto sp : this->mOnlineSpMap) {
        if (sp.second->mSaveData.lastSpawnerUuid == uuid) spawnCount++;
        if (sp.second->mSaveData.ownerUuid == uuid) OwnCount++;
    }
    if (spawnCount >= this->mConfig.maxOnlinePerPlayer)
        return base::OperateResult::error(
            "manager.fail.tooManyOnlinePerPlayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
        );
    // check: maxOwn
    for (auto sp : this->mOfflineSpMap) {
        if (sp.second->mSaveData.ownerUuid == uuid) OwnCount++;
    }
    if (OwnCount >= this->mConfig.maxOwn) return base::OperateResult::error("manager.fail.tooManyOwnSp"_tr());
    return base::OperateResult::swing();
}

base::OperateResult CFSPManager::canSpawnPlayer(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSpawn);
        checkResult.mType != base::OperateResult::Type::Swing)
        return checkResult;
    // check: maxOnline
    auto uuid = player ? player->getUuid().asString() : "";
    if (this->mOnlineSpMap.size() >= this->mConfig.maxOnline)
        return base::OperateResult::error("manager.fail.tooManyOnline"_tr(std::to_string(this->mConfig.maxOnline)));
    // check: maxOnlinePerPlayer
    unsigned long long spawnCount = 0;
    for (auto sp : this->mOnlineSpMap)
        if (sp.second->mSaveData.lastSpawnerUuid == uuid) spawnCount++;
    if (spawnCount >= this->mConfig.maxOnlinePerPlayer)
        return base::OperateResult::error(
            "manager.fail.tooManyOnlinePerPlayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
        );
    return base::OperateResult::swing();
}

base::OperateResult CFSPManager::canCreateGroup(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupCreate)) return checkResult;
    auto               uuid  = player->getUuid().asString();
    unsigned long long count = 0;
    for (auto group : this->mGroupMap) {
        if (group.second->mData.ownerUuid == uuid) count++;
    }
    if (count >= this->mConfig.maxGroup) return base::OperateResult::error("manager.fail.tooManyOwnGroup"_tr());
    return base::OperateResult::swing();
}

uint CFSPManager::getSpBasePermissionMask() {
    uint perm = 0;
    if (this->mPermissionConfig.spSpawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Spawn;
    if (this->mPermissionConfig.spDespawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Despawn;
    if (this->mPermissionConfig.spRespawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Respawn;
    if (this->mPermissionConfig.spDelete.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Delete;
    if (this->mPermissionConfig.spStop.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Stop;
    if (this->mPermissionConfig.spDrop.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Drop;
    if (this->mPermissionConfig.spDropInv.enabled) perm |= (uint)simulated_player::SimPlayerPermission::DropInv;
    if (this->mPermissionConfig.spSwap.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Swap;
    if (this->mPermissionConfig.spSneaking.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Sneaking;
    if (this->mPermissionConfig.spSwimming.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Swimming;
    if (this->mPermissionConfig.spFlying.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Flying;
    if (this->mPermissionConfig.spSprinting.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Sprinting;
    if (this->mPermissionConfig.spAttack.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Attack;
    if (this->mPermissionConfig.spBuild.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Build;
    if (this->mPermissionConfig.spInteract.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Interact;
    if (this->mPermissionConfig.spJump.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Jump;
    if (this->mPermissionConfig.spUse.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Use;
    if (this->mPermissionConfig.spDestroy.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Destroy;
    if (this->mPermissionConfig.spChat.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Chat;
    if (this->mPermissionConfig.spRunCmd.enabled) perm |= (uint)simulated_player::SimPlayerPermission::RunCmd;
    if (this->mPermissionConfig.spSelect.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Select;
    if (this->mPermissionConfig.spLookAt.enabled) perm |= (uint)simulated_player::SimPlayerPermission::LookAt;
    if (this->mPermissionConfig.spMoveTo.enabled) perm |= (uint)simulated_player::SimPlayerPermission::MoveTo;
    if (this->mPermissionConfig.spNavTo.enabled) perm |= (uint)simulated_player::SimPlayerPermission::NavTo;
    if (this->mPermissionConfig.spTp.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Tp;
    return perm;
}

uint CFSPManager::getSpPermissionMask(CommandPermissionLevel level) {
    uint perm = 0;
    if (level >= this->mPermissionConfig.spSpawn.permission) perm |= (uint)simulated_player::SimPlayerPermission::Spawn;
    if (level >= this->mPermissionConfig.spDespawn.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Despawn;
    if (level >= this->mPermissionConfig.spRespawn.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Respawn;
    if (level >= this->mPermissionConfig.spDelete.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Delete;
    if (level >= this->mPermissionConfig.spStop.permission) perm |= (uint)simulated_player::SimPlayerPermission::Stop;
    if (level >= this->mPermissionConfig.spDrop.permission) perm |= (uint)simulated_player::SimPlayerPermission::Drop;
    if (level >= this->mPermissionConfig.spDropInv.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::DropInv;
    if (level >= this->mPermissionConfig.spSwap.permission) perm |= (uint)simulated_player::SimPlayerPermission::Swap;
    if (level >= this->mPermissionConfig.spSneaking.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Sneaking;
    if (level >= this->mPermissionConfig.spSwimming.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Swimming;
    if (level >= this->mPermissionConfig.spFlying.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Flying;
    if (level >= this->mPermissionConfig.spSprinting.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Sprinting;
    if (level >= this->mPermissionConfig.spAttack.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Attack;
    if (level >= this->mPermissionConfig.spBuild.permission) perm |= (uint)simulated_player::SimPlayerPermission::Build;
    if (level >= this->mPermissionConfig.spInteract.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Interact;
    if (level >= this->mPermissionConfig.spJump.permission) perm |= (uint)simulated_player::SimPlayerPermission::Jump;
    if (level >= this->mPermissionConfig.spUse.permission) perm |= (uint)simulated_player::SimPlayerPermission::Use;
    if (level >= this->mPermissionConfig.spDestroy.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Destroy;
    if (level >= this->mPermissionConfig.spChat.permission) perm |= (uint)simulated_player::SimPlayerPermission::Chat;
    if (level >= this->mPermissionConfig.spRunCmd.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::RunCmd;
    if (level >= this->mPermissionConfig.spSelect.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Select;
    if (level >= this->mPermissionConfig.spLookAt.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::LookAt;
    if (level >= this->mPermissionConfig.spMoveTo.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::MoveTo;
    if (level >= this->mPermissionConfig.spNavTo.permission) perm |= (uint)simulated_player::SimPlayerPermission::NavTo;
    if (level >= this->mPermissionConfig.spTp.permission) perm |= (uint)simulated_player::SimPlayerPermission::Tp;
    return perm;
}

uint CFSPManager::getSpPermissionMask(std::optional<CommandPermissionLevel> level) {
    static uint baseMask = this->getSpBasePermissionMask();
    if (!level.has_value()) return baseMask;
    switch (level.value()) {
    case CommandPermissionLevel::Any:
        static uint maskAny = baseMask & getSpPermissionMask(level.value());
        return maskAny;
    case CommandPermissionLevel::GameDirectors:
        static uint maskGameDirectors = baseMask & getSpPermissionMask(level.value());
        return maskGameDirectors;
    case CommandPermissionLevel::Admin:
        static uint maskAdmin = baseMask & getSpPermissionMask(level.value());
        return maskAdmin;
    case CommandPermissionLevel::Host:
        static uint maskHost = baseMask & getSpPermissionMask(level.value());
        return maskHost;
    case CommandPermissionLevel::Owner:
        static uint maskOwner = baseMask & getSpPermissionMask(level.value());
        return maskOwner;
    case CommandPermissionLevel::Internal:
        static uint maskInternal = baseMask & getSpPermissionMask(level.value());
        return maskInternal;
    }
    return 0;
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

base::OperateResult CFSPManager::spPerm(
    const Player*              player,
    std::string                spname,
    uint                       perm,
    bool                       enable,
    std::optional<std::string> targetUUid
) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = targetUUid.has_value() ? this->baseCheck(player, this->mPermissionConfig.spPerm)
                                              : this->baseCheck(player, this->mPermissionConfig.spPublicPerm);
    if (!checkResult) return checkResult;
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        it = this->mOfflineSpMap.find(spname);
        if (it == this->mOfflineSpMap.end()) return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (checkResult.mType != base::OperateResult::Type::Success
        && it->second->mSaveData.ownerUuid != player->getUuid().asString())
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (targetUUid.has_value()) {
        if (enable) {
            auto permIt = it->second->mSaveData.permission.find(targetUUid.value());
            if (permIt == it->second->mSaveData.permission.end())
                it->second->mSaveData.permission[targetUUid.value()] = perm;
            else if (permIt->second & perm) return base::OperateResult::error("manager.fail.targetHasHadPerm"_tr());
            else it->second->mSaveData.permission[targetUUid.value()] |= perm;
        } else {
            auto permIt = it->second->mSaveData.permission.find(targetUUid.value());
            if (permIt == it->second->mSaveData.permission.end() || !(permIt->second & perm))
                return base::OperateResult::error("manager.fail.targetNotHavePerm"_tr());
            permIt->second &= ~perm;
            if (permIt->second == 0) it->second->mSaveData.permission.erase(permIt);
        }
    } else {
        if (enable) {
            if (it->second->mSaveData.publicPermission & perm)
                return base::OperateResult::error("manager.fail.targetHasHadPerm"_tr());
            it->second->mSaveData.publicPermission |= perm;
        } else {
            if (!(it->second->mSaveData.publicPermission & perm))
                return base::OperateResult::error("manager.fail.targetNotHavePerm"_tr());
            it->second->mSaveData.publicPermission &= ~perm;
        }
    }
    it->second->mShouldSave = true;
    if (!it->second->mSimPlayer) it->second->save();
    return base::OperateResult::success("manager.success.set"_tr());
}

base::OperateResult CFSPManager::groupPerm(
    const Player*              player,
    std::string                spname,
    uint                       perm,
    bool                       enable,
    std::optional<std::string> targetUUid
) {
    using ll::i18n_literals::operator""_tr;
    auto checkResult = targetUUid.has_value() ? this->baseCheck(player, this->mPermissionConfig.groupPerm)
                                              : this->baseCheck(player, this->mPermissionConfig.groupPublicPerm);
    if (!checkResult) return checkResult;
    auto it = this->mGroupMap.find(spname);
    if (it == this->mGroupMap.end()) return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    if (checkResult.mType != base::OperateResult::Type::Success
        && it->second->mData.ownerUuid != player->getUuid().asString())
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (targetUUid.has_value()) {
        if (enable) {
            auto permIt = it->second->mData.permission.find(targetUUid.value());
            if (permIt == it->second->mData.permission.end()) it->second->mData.permission[targetUUid.value()] = perm;
            else if (permIt->second & perm) return base::OperateResult::error("manager.fail.targetHasHadPerm"_tr());
            else it->second->mData.permission[targetUUid.value()] |= perm;
        } else {
            auto permIt = it->second->mData.permission.find(targetUUid.value());
            if (permIt == it->second->mData.permission.end() || !(permIt->second & perm))
                return base::OperateResult::error("manager.fail.targetNotHavePerm"_tr());
            permIt->second &= ~perm;
            if (permIt->second == 0) it->second->mData.permission.erase(permIt);
        }
    } else {
        if (enable) {

            if (it->second->mData.publicPermission & perm)
                return base::OperateResult::error("manager.fail.targetHasHadPerm"_tr());
            it->second->mData.publicPermission |= perm;
        } else {
            if (!(it->second->mData.publicPermission & perm))
                return base::OperateResult::error("manager.fail.targetNotHavePerm"_tr());
            it->second->mData.publicPermission &= ~perm;
        }
    }
    it->second->save();
    return base::OperateResult::success("manager.success.set"_tr());
}
} // namespace coral_fans::cfsp::manager