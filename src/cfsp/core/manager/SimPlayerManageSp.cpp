#include "SimPlayerManager.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/Minecraft.h"
#include <memory>

namespace coral_fans::cfsp::manager {
base::CommandResult SimPlayerManager::createSp(
    Player*            player,
    std::string const& name,
    Vec3 const&        pos,
    DimensionType      dim,
    bool               isLockUniqueId
) {
    using ll::i18n_literals::operator""_tr;
    // check: isSimulatedPlayer
    if (player && player->isSimulatedPlayer()) return base::CommandResult();
    std::string spname    = this->mConfig.namePrefix + name + this->mConfig.namePostfix;
    auto        ownerUuid = player->getUuid().asString();
    // check: already exist
    if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end()
        || this->mOfflineSpDataMap.find(spname) != this->mOfflineSpDataMap.end()) {
        return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.exist"_tr());
    }
    if (!isManager(player)) {
        // check: maxOnline
        if (this->mOnlineCount >= this->mConfig.maxOnline)
            return base::CommandResult(
                base::CommandResult::Type::error,
                "translate.simplayer.error.toomanyonline"_tr(std::to_string(this->mConfig.maxOnline))
            );
        // check: maxOnlinePerPlayer
        if (this->mOnlineCountPerPlayer[ownerUuid] >= this->mConfig.maxOnlinePerPlayer)
            return base::CommandResult(
                base::CommandResult::Type::error,
                "translate.simplayer.error.toomanyonlineperplayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
            );
        // check: maxOwn
        unsigned long long count = 0;
        for (auto sp : this->mOnlineSpMap) {
            if (sp.second->mSaveData.ownerUuid == ownerUuid) count++;
        }
        for (auto spdata : this->mOfflineSpDataMap) {
            if (spdata.second->ownerUuid == ownerUuid) count++;
        }
        if (count >= this->mConfig.maxOwn)
            return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.toomanyown"_tr());
    }
    // create
    auto mc = ll::service::getMinecraft();
    if (!mc)
        return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.cannotcreate"_tr());
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler)
        return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.cannotcreate"_tr());
    auto  xuid      = "-" + std::to_string(std::hash<std::string>()(spname));
    auto* simPlayer = SimulatedPlayer::create(
        spname,
        {0, 0, 0},
        pos,
        {0, 0},
        isLockUniqueId,
        dim,
        serverNetworkHandler,
        xuid,
        std::nullopt
    );
    if (!simPlayer)
        return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.failedtocreate"_tr());

    simPlayer->mPlayerRespawnPoint->mPlayerPosition = pos;
    simPlayer->mPlayerRespawnPoint->mDimension      = dim;

    simulated_player::SimPlayerSaveData saveData;
    saveData.name              = spname;
    saveData.uniqueId          = simPlayer->getOrCreateUniqueID().rawID;
    saveData.xuid              = xuid;
    saveData.ownerUuid         = ownerUuid;
    saveData.isLockUniqueId    = isLockUniqueId;
    saveData.lastSpawnerUuid   = ownerUuid;
    this->mOnlineSpMap[spname] = std::make_shared<simulated_player::SimPlayer>(saveData, simPlayer);
    ++this->mOnlineCountPerPlayer[ownerUuid];
    ++this->mOnlineCount;
    return base::CommandResult(base::CommandResult::Type::success, "translate.simplayer.success"_tr());
}
} // namespace coral_fans::cfsp::manager