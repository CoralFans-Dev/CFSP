#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/network/packet/TextPacket.h"
#include "mc/world/Minecraft.h"
#include <memory>
#include <optional>
#include <vector>

namespace coral_fans::cfsp::manager {
#define IF_HAS_ARGS(...) , ##__VA_ARGS__

#define SP_ONLINE_FUNC_DEF(FUNC, ACTION, ...)                                                                          \
    base::OperateResult CFSPManager::sp##FUNC(                                                                         \
        Player*                   player,                                                                              \
        std::string const& spname IF_HAS_ARGS(__VA_ARGS__),                                                            \
        bool                      nocheck                                                                              \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        if (!nocheck) {                                                                                                \
            if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.sp##FUNC); !checkResult)            \
                return checkResult;                                                                                    \
            else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;                          \
        }                                                                                                              \
        auto it = this->mOnlineSpMap.find(spname);                                                                     \
        if (it == this->mOnlineSpMap.end()) {                                                                          \
            if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())                                         \
                return base::OperateResult::error("manager.fail.spHasOffline"_tr());                                   \
            return base::OperateResult::error("manager.fail.spNotExisted"_tr());                                       \
        }                                                                                                              \
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::FUNC))               \
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());                                   \
        return it->second->ACTION;                                                                                     \
    }

void CFSPManager::saveSps() {
    for (auto cfsp : this->mOnlineSpMap) cfsp.second->save();
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

std::string CFSPManager::listOnlineSp(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (!player) {
        res = "manager.info.onlineCount"_tr(this->mOnlineSpMap.size());
        for (auto cfsp : this->mOnlineSpMap)
            res += "manager.info.onlineSp"_tr(
                cfsp.first,
                base::utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toJsonString()
            );
        return res;
    }
    auto uuid  = player->getUuid().asString();
    int  count = 0;
    for (auto cfsp : this->mOnlineSpMap)
        if (cfsp.second->mSaveData.ownerUuid == uuid) {
            res += "manager.info.onlineSp"_tr(
                cfsp.first,
                base::utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toJsonString()
            );
            count++;
        }
    res = "manager.info.onlineCountPerplayer"_tr(count) + res;
    return res;
}

std::string CFSPManager::listOfflineSp(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (!player) {
        res = "manager.info.offlineCount"_tr(this->mOnlineSpMap.size());
        for (auto cfsp : this->mOfflineSpMap)
            res += "manager.info.offlineSp"_tr(
                cfsp.first,
                base::utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toJsonString()
            );
        return res;
    }
    auto uuid  = player->getUuid().asString();
    int  count = 0;
    for (auto cfsp : this->mOfflineSpMap)
        if (cfsp.second->mSaveData.ownerUuid == uuid) {
            res += "manager.info.oflineSp"_tr(
                cfsp.first,
                base::utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toJsonString()
            );
            count++;
        }
    res = "manager.info.offlineCountPerplayer"_tr(count) + res;
    return res;
}

base::OperateResult
CFSPManager::spCreate(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim, bool lockUniqueId) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreatePlayer(player); !checkResult) return checkResult;
    std::string spname    = this->mConfig.namePrefix + name + this->mConfig.namePostfix;
    auto        ownerUuid = player ? player->getUuid().asString() : "";
    // check: already exist
    if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end()
        || this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end()) {
        return base::OperateResult::error("manager.fail.spHasExisted"_tr());
    }
    // check: name
    if (!this->tryCreateDiretory(cfsp::CFSP::getInstance().getSelf().getDataDir() / "simplayer", spname))
        return base::OperateResult::error("manager.fail.includeIllegalChar"_tr());
    // create
    auto simplayer = simulated_player::SimPlayer::create(player, spname, pos, dim, lockUniqueId);
    if (!simplayer) [[unlikely]]
        return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->mOnlineSpMap[spname] = simplayer;

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspSplist", {spname});

    return base::OperateResult::success("manager.success.create"_tr());
}

base::OperateResult CFSPManager::spSpawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->canSpawnPlayer(player); !checkResult) return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    // check: exist
    auto it = this->mOfflineSpMap.find(spname);
    if (it == this->mOfflineSpMap.end()) {
        if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOnline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    // check：permission
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Spawn))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    // create
    auto res = it->second->spawn(player);
    if (!res) [[unlikely]]
        return res;

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOfflineSp", {spname});
    if (it->second->mSimPlayer->isDead())
        ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspDeadSp", {it->first});

    if (auto node = mOfflineSpMap.extract(it)) {
        mOnlineSpMap.insert(std::move(node));
    }
    return res;
}

void CFSPManager::autoJoin() {
    std::vector<std::string> spawnlist;
    std::vector<std::string> spawnDeadlist;
    auto                     it = this->mOfflineSpMap.begin();
    while (it != this->mOfflineSpMap.end()) {
        if (it->second->mSaveData.isOnline && it->second->spawn(std::nullopt)) {
            spawnlist.emplace_back(it->first);
            if (it->second->mSimPlayer->isDead()) spawnDeadlist.emplace_back(it->first);
            auto current_it = it++;
            auto node       = this->mOfflineSpMap.extract(current_it);
            mOnlineSpMap.insert(std::move(node));
            continue;
        }
        it++;
    }
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", spawnlist);
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOfflineSp", spawnlist);
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspDeadSp", spawnDeadlist);
}

base::OperateResult CFSPManager::spDespawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spDespawn); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    // check：permission
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Despawn))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    auto isDead = it->second->mSimPlayer && it->second->mSimPlayer->isDead();
    auto res    = it->second->despawn();
    if (!res) [[unlikely]]
        return res;

    TextPacket::createRawMessage("manager.success.spOffline"_tr(spname)).sendToClients();

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOfflineSp", {spname});
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOnlineSp", {spname});
    if (isDead)
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspDeadSp", {it->second->mSaveData.name});

    if (auto node = mOnlineSpMap.extract(it)) mOfflineSpMap.insert(std::move(node));
    return res;
}

void CFSPManager::autoDespawn(std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    if (!cfsp->mSimPlayer) return;
    cfsp->despawn();
    auto spname = cfsp->mSaveData.name;
    TextPacket::createRawMessage("manager.success.autoDespawn"_tr(spname)).sendToClients();
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOfflineSp", {spname});
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOnlineSp", {spname});

    if (auto node = mOnlineSpMap.extract(spname)) mOfflineSpMap.insert(std::move(node));
}

base::OperateResult CFSPManager::spRespawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spRespawn); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    // check：permission
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Respawn))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    auto res = it->second->respawn();
    if (!res) [[unlikely]]
        return res;
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspDeadSp", {it->second->mSaveData.name});
    return res;
}

base::OperateResult CFSPManager::spDelete(Player* player, std::string const& spname, bool force, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spDelete); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    if (auto it = this->mOnlineSpMap.find(spname); it != this->mOnlineSpMap.end()) {
        // check：permission
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Delete))
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (!force && !it->second->mSaveData.isEmptyInv)
            return base::OperateResult::swing("manager.fail.notEmpty"_tr(spname));
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspSplist", {spname});
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOnlineSp", {spname});
        if (!it->second->mSimPlayer || it->second->mSimPlayer->isDead())
            ll::command::CommandRegistrar::getInstance().removeSoftEnumValues(
                "cfspDeadSp",
                {it->second->mSaveData.name}
            );
        it->second->stop();
        if (it->second->mSimPlayer) {
            it->second->mSimPlayer->disconnect();
            it->second->mSimPlayer->remove();
            it->second->mSimPlayer->setGameTestHelper(nullptr);
        }
        std::filesystem::remove_all(
            CFSP::getInstance().getSelf().getDataDir() / "simplayer"
            / reinterpret_cast<const char8_t*>(it->second->mSaveData.name.c_str())
        );
        this->mOnlineSpMap.erase(it);
    } else if (it = this->mOfflineSpMap.find(spname); it != this->mOfflineSpMap.end()) {
        // check：permission
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Delete))
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (!force && !it->second->mSaveData.isEmptyInv)
            return base::OperateResult::swing("manager.fail.notEmpty"_tr(spname));
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspSplist", {spname});
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOfflineSp", {spname});
        std::filesystem::remove_all(
            CFSP::getInstance().getSelf().getDataDir() / "simplayer"
            / reinterpret_cast<const char8_t*>(it->second->mSaveData.name.c_str())
        );
        this->mOfflineSpMap.erase(it);
    } else return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    for (auto group : this->mGroupMap) {
        if (auto it = group.second->mData.splist.find(spname); it != group.second->mData.splist.end()) {
            group.second->mData.splist.erase(it);
            group.second->save();
        }
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult CFSPManager::spInfo(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (isManager(player)) nocheck = true;
    }
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::None))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return it->second->info();
}

base::OperateResult CFSPManager::spInvInfo(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (isManager(player)) nocheck = true;
    }
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::None))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return it->second->invInfo();
}

SP_ONLINE_FUNC_DEF(Stop, stop())
SP_ONLINE_FUNC_DEF(LookAt, lookAt(pos), Vec3 const& pos)

SP_ONLINE_FUNC_DEF(Drop, drop())
SP_ONLINE_FUNC_DEF(DropInv, dropInv())
SP_ONLINE_FUNC_DEF(Swap, swap(player))
SP_ONLINE_FUNC_DEF(Select, select(id), int id)

SP_ONLINE_FUNC_DEF(Chat, chat(message), std::string const& message)
SP_ONLINE_FUNC_DEF(RunCmd, runcmd(message), std::string const& message)

SP_ONLINE_FUNC_DEF(MoveTo, moveTo(pos, speed), Vec3 const& pos, float speed)
SP_ONLINE_FUNC_DEF(NavTo, navTo(pos, speed), Vec3 const& pos, float speed)
SP_ONLINE_FUNC_DEF(Tp, tp(pos, dimId), Vec3 pos, std::optional<int> dimId)

SP_ONLINE_FUNC_DEF(Attack, attack(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(Build, build(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(Interact, interact(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(Jump, jump(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(Use, use(_long, times, interval), int _long, int times, int interval)
SP_ONLINE_FUNC_DEF(Destroy, destroy(_long, times, interval), int _long, int times, int interval)

SP_ONLINE_FUNC_DEF(Sneaking, sneaking(enabled), std::optional<bool> enabled)
SP_ONLINE_FUNC_DEF(Swimming, swimming(enabled), std::optional<bool> enabled)
SP_ONLINE_FUNC_DEF(Flying, flying(enabled), std::optional<bool> enabled)
SP_ONLINE_FUNC_DEF(Sprinting, sprinting(enabled), std::optional<bool> enabled)
} // namespace coral_fans::cfsp::manager