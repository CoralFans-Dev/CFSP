#include "CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/Minecraft.h"
#include <memory>


namespace coral_fans::cfsp::manager {
base::OperateResult CFSPManager::createSp(
    Player*            player,
    std::string const& name,
    Vec3 const&        pos,
    DimensionType      dim,
    bool               isLockUniqueId
) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreatePlayer(player); checkResult.mType != base::OperateResult::Type::success)
        return checkResult;
    std::string spname    = this->mConfig.namePrefix + name + this->mConfig.namePostfix;
    auto        ownerUuid = player->getUuid().asString();
    // check: already exist
    if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end()
        || this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end()) {
        return base::OperateResult::error("manager.fail.spHasExisted"_tr());
    }
    // create
    auto mc = ll::service::getMinecraft();
    if (!mc) return base::OperateResult::error("manager.error.failedtocreate"_tr());
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return base::OperateResult::error("manager.error.failedtocreate"_tr());
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
    if (!simPlayer) return base::OperateResult::error("manager.error.failedtocreate"_tr());

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

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspname", {spname});

    return base::OperateResult::success("manager.success.create"_tr());
}
} // namespace coral_fans::cfsp::manager