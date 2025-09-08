#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/Config.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/network/packet/TextPacket.h"
#include "mc/world/Minecraft.h"
#include <memory>
#include <optional>
#include <vector>

namespace coral_fans::cfsp::manager {
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

base::OperateResult CFSPManager::spCreate(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim) {
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
    auto simplayer = simulated_player::SimPlayer::create(player, spname, pos, dim);
    if (!simplayer) [[unlikely]]
        return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->mOnlineSpMap[spname] = simplayer;

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspSplist", {spname});

    return base::OperateResult::success("manager.success.create"_tr());
}

base::OperateResult CFSPManager::spSpawn(Player* player, std::string const& spname, bool nocheck, bool isLockUniqueId) {
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
    if (!nocheck)
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Spawn); !res)
            return res;
    // create
    auto res = it->second->spawn(player, isLockUniqueId);
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
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Despawn); !res)
            return res;
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
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Respawn); !res)
            return res;
    auto res = it->second->respawn();
    if (!res) [[unlikely]]
        return res;
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspDeadSp", {it->second->mSaveData.name});
    return res;
}

base::OperateResult CFSPManager::spRm(Player* player, std::string const& spname, bool nocheck, bool force) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spRm); !checkResult) return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    if (auto it = this->mOnlineSpMap.find(spname); it != this->mOnlineSpMap.end()) {
        if (!nocheck)
            if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Rm); !res)
                return res;
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
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / it->second->mSaveData.name
        );
        this->mOnlineSpMap.erase(it);
    } else if (it = this->mOfflineSpMap.find(spname); it != this->mOfflineSpMap.end()) {
        if (!nocheck)
            if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Rm); !res)
                return res;
        if (!force && !it->second->mSaveData.isEmptyInv)
            return base::OperateResult::swing("manager.fail.notEmpty"_tr(spname));
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspSplist", {spname});
        ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOfflineSp", {spname});
        std::filesystem::remove_all(
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / it->second->mSaveData.name
        );
        this->mOfflineSpMap.erase(it);
    } else return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    for (auto group : this->mGroupMap) {
        if (auto it = group.second->mData.splist.find(spname); it != group.second->mData.splist.end()) {
            group.second->mData.splist.erase(it);
            ll::config::saveConfig(
                group.second->mData,
                CFSP::getInstance().getSelf().getDataDir() / "group" / group.second->mData.name / "data.json"
            );
        }
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult CFSPManager::spStop(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spStop); !checkResult)
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
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Stop); !res) return res;
    return it->second->stop();
}

base::OperateResult CFSPManager::spLookAt(Player* player, std::string const& spname, Vec3 const& pos, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spLookAt); !checkResult)
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
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::LookAt); !res)
            return res;
    return it->second->lookAt(pos);
}
} // namespace coral_fans::cfsp::manager