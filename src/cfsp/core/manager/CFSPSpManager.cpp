#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/network/packet/TextPacket.h"
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
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.sp##FUNC); !checkResult)                \
            return checkResult;                                                                                        \
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;                              \
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

std::string CFSPManager::listOnlineSp(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (!player) {
        res = "manager.info.onlineCount"_tr(this->mOnlineSpMap.size());
        for (auto cfsp : this->mOnlineSpMap)
            res += "manager.info.onlineSp"_tr(
                cfsp.first,
                base::utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toString()
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
                cfsp.second->mSimPlayer->getFeetPos().toString()
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
                cfsp.second->mSimPlayer->getFeetPos().toString()
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
                cfsp.second->mSimPlayer->getFeetPos().toString()
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

    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspSplist", {spname});

    return base::OperateResult::success("manager.success.create"_tr());
}

base::OperateResult CFSPManager::spSpawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canSpawnPlayer(player); !checkResult) return checkResult;
    else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
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

    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOfflineSp", {spname});
    if (it->second->mSimPlayer->isDead())
        ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspDeadSp", {it->first});

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
        if (it->second->mSaveData.isOnline) {
            auto res = it->second->spawn(std::nullopt);
            CFSP::getInstance().getSelf().getLogger().info(res.mInfo);
            if (res) {
                spawnlist.emplace_back(it->first);
                if (it->second->mSimPlayer->isDead()) spawnDeadlist.emplace_back(it->first);
                auto current_it = it++;
                auto node       = this->mOfflineSpMap.extract(current_it);
                mOnlineSpMap.insert(std::move(node));
                continue;
            }
        }
        it++;
    }
    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspOnlineSp", spawnlist);
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOfflineSp", spawnlist);
    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspDeadSp", spawnDeadlist);
}

base::OperateResult CFSPManager::spDespawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spDespawn); !checkResult) return checkResult;
    else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
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

    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspOfflineSp", {spname});
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOnlineSp", {spname});
    if (isDead)
        ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues(
            "cfspDeadSp",
            {it->second->mSaveData.name}
        );

    if (auto node = mOnlineSpMap.extract(it)) mOfflineSpMap.insert(std::move(node));
    return res;
}

void CFSPManager::autoDespawn(std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    if (!cfsp->mSimPlayer) return;
    cfsp->despawn();
    auto spname = cfsp->mSaveData.name;
    TextPacket::createRawMessage("manager.success.autoDespawn"_tr(spname)).sendToClients();
    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues("cfspOfflineSp", {spname});
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOnlineSp", {spname});

    if (auto node = mOnlineSpMap.extract(spname)) mOfflineSpMap.insert(std::move(node));
}

base::OperateResult CFSPManager::spRespawn(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spRespawn); !checkResult) return checkResult;
    else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
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
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspDeadSp", {it->second->mSaveData.name});
    return res;
}

base::OperateResult CFSPManager::spDelete(Player* player, std::string const& spname, bool force, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spDelete); !checkResult) return checkResult;
    else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    bool deleteFail = false;
    if (auto it = this->mOnlineSpMap.find(spname); it != this->mOnlineSpMap.end()) {
        // check：permission
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Delete))
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (!force && !it->second->isEmptyInv()) return base::OperateResult::swing("manager.fail.notEmpty"_tr(spname));
        ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspSplist", {spname});
        ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOnlineSp", {spname});
        if (!it->second->mSimPlayer || it->second->mSimPlayer->isDead())
            ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues(
                "cfspDeadSp",
                {it->second->mSaveData.name}
            );
        it->second->stop();
        if (it->second->mSimPlayer) {
            it->second->mSimPlayer->disconnect();
            it->second->mSimPlayer->remove();
            it->second->mSimPlayer->setGameTestHelper(nullptr);
        }
        try {
            std::filesystem::remove_all(
                CFSP::getInstance().getSelf().getDataDir() / "simplayer"
                / reinterpret_cast<const char8_t*>(it->second->mSaveData.name.c_str())
            );
        } catch (...) {
            deleteFail = true;
        }
        this->mOnlineSpMap.erase(it);
    } else if (it = this->mOfflineSpMap.find(spname); it != this->mOfflineSpMap.end()) {
        // check：permission
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Delete))
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (!force && !it->second->checkInvEmptyForOfflineCFSP())
            return base::OperateResult::swing("manager.fail.notEmpty"_tr(spname));
        ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspSplist", {spname});
        ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues("cfspOfflineSp", {spname});
        try {
            std::filesystem::remove_all(
                CFSP::getInstance().getSelf().getDataDir() / "simplayer"
                / reinterpret_cast<const char8_t*>(it->second->mSaveData.name.c_str())
            );
            this->mOfflineSpMap.erase(it);
        } catch (...) {
            deleteFail = true;
        }
    } else return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    for (auto group : this->mGroupMap) {
        if (auto it = group.second->mData.splist.find(spname); it != group.second->mData.splist.end()) {
            group.second->mData.splist.erase(it);
            group.second->save();
        }
    }
    if (deleteFail) return base::OperateResult::error("manager.error.deleteFileFail"_tr());
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult CFSPManager::spInfo(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (isManager(player)) nocheck = true;
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        it = this->mOfflineSpMap.find(spname);
        if (it == this->mOfflineSpMap.end()) return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::None))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return it->second->info();
}

base::OperateResult CFSPManager::spInvInfo(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (isManager(player)) nocheck = true;
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
SP_ONLINE_FUNC_DEF(LookAt, lookAt(direction), simulated_player::SimPlayer::Direction direction)

SP_ONLINE_FUNC_DEF(Drop, drop(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(DropInv, dropInv(times, interval), int times, int interval)
SP_ONLINE_FUNC_DEF(Swap, swap(player))
SP_ONLINE_FUNC_DEF(Select, select(id), int id)
SP_ONLINE_FUNC_DEF(OpenInv, openInv(player))

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