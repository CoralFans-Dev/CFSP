#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/Minecraft.h"
#include <memory>

namespace coral_fans::cfsp::manager {
std::string CFSPManager::listOnlineSp(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    std::string res = "";
    if (!player) {
        res = "manager.info.onlineCount"_tr(this->mOnlineSpMap.size());
        for (auto cfsp : this->mOnlineSpMap)
            res += "manager.info.onlineSp"_tr(
                cfsp.first,
                utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
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
                utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
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
                utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
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
                utils::tryGetPlayerName(cfsp.second->mSaveData.ownerUuid),
                cfsp.second->mSimPlayer->getFeetPos().toJsonString()
            );
            count++;
        }
    res = "manager.info.offlineCountPerplayer"_tr(count) + res;
    return res;
}

base::OperateResult CFSPManager::createSp(
    Player*            player,
    std::string const& name,
    Vec3 const&        pos,
    DimensionType      dim,
    bool               isLockUniqueId
) {
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
    auto simplayer = simulated_player::SimPlayer::create(player, spname, pos, dim, isLockUniqueId);
    if (!simplayer) return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->mOnlineSpMap[spname] = simplayer;

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspSplist", {spname});

    return base::OperateResult::success("manager.success.create"_tr());
}

CFSP_API base::OperateResult CFSPManager::spawnSp(Player* player, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spawnSp); !checkResult) return checkResult;
    // check: exist
    auto it = this->mOfflineSpMap.find(spname);
    if (it == this->mOfflineSpMap.end()) {
        if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOnline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    // check：permission
    if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Spawn); !res) return res;
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
    // create
    if (!it->second->spawn(player)) return base::OperateResult::error("manager.error.failedtocreate"_tr());

    if (auto node = mOfflineSpMap.extract(it)) {
        mOnlineSpMap.insert(std::move(node));
    }

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspOnlineSp", {spname});
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues("cfspOfflineSp", {spname});

    return base::OperateResult::success("manager.success.operate"_tr());
}

CFSP_API base::OperateResult CFSPManager::despawnSp(Player* player, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.despawnSp); !checkResult) return checkResult;
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    // check：permission
    if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Despawn); !res) return res;
}
} // namespace coral_fans::cfsp::manager