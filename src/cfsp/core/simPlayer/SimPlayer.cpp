#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Schedule.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/fix/CFSPFixManager.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/server/sim/LookDuration.h"
#include "mc/world/Minecraft.h"
#include "mc/world/level/Level.h"


// #include "mc/world/actor/provider/ActorAttribute.h"
#include <optional>


namespace coral_fans::cfsp::simulated_player {
void SimPlayer::cancelTask() { base::Schedule::getInstance().getSchedule()->cancel(this->mTaskid); }

void SimPlayer::cancelScript() { base::Schedule::getInstance().getSchedule()->cancel(this->mScriptid); }

base::OperateResult SimPlayer::stop() {
    using ll::i18n_literals::operator""_tr;
    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mTaskid)) this->cancelTask();
    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mScriptid)) this->cancelScript();

    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());

    this->mSimPlayer->simulateStopDestroyingBlock();

    this->mSimPlayer->simulateStopInteracting();

    this->mSimPlayer->simulateStopMoving();

    this->mSimPlayer->simulateStopUsingItem();

    return base::OperateResult::success("manager.success.operate"_tr());
}

std::shared_ptr<SimPlayer> SimPlayer::create(
    const Player*      player,
    std::string const& spname,
    Vec3 const&        pos,
    DimensionType      dim,
    bool               lockUniqueId
) {
    auto uuid = player ? player->getUuid().asString() : "";
    auto mc   = ll::service::getMinecraft();
    if (!mc) return nullptr;
    auto handler = mc->getServerNetworkHandler();
    if (!handler) return nullptr;
    auto xuid = "-" + std::to_string(std::hash<std::string>()(spname));

    fix::CFSPFixManager::getInstance().createSpMutex = true;
    auto simPlayer = SimulatedPlayer::create(spname, pos, {0, 0, 0}, {0, 0}, false, dim, handler, xuid, std::nullopt);
    fix::CFSPFixManager::getInstance().createSpMutex = false;

    if (!simPlayer) [[unlikely]]
        return nullptr;

    simPlayer->teleport(pos, dim);
    simPlayer->mPlayerRespawnPoint->mPlayerPosition = pos;
    simPlayer->mPlayerRespawnPoint->mDimension      = dim;

    SimPlayerSaveData saveData;
    saveData.name = spname;
    if (lockUniqueId) saveData.uniqueId = simPlayer->getOrCreateUniqueID().rawID;
    saveData.xuid            = xuid;
    saveData.ownerUuid       = uuid;
    saveData.lastSpawnerUuid = uuid;

    const auto& gameType = player->getPlayerGameType();
    simPlayer->setPlayerGameType(gameType);
    saveData.mGameType = gameType;

    auto cfsp = std::make_shared<simulated_player::SimPlayer>(saveData, simPlayer);

    cfsp->lookAt(
        player ? simPlayer->getHeadPos() + Vec3::directionFromRotation(player->getRotation()) : simPlayer->getHeadPos()
    );

    return cfsp;
}

base::OperateResult SimPlayer::spawn(std::optional<const Player*> player) {
    using ll::i18n_literals::operator""_tr;
    auto mc = ll::service::getMinecraft();
    if (!mc) return base::OperateResult::error("manager.error.failedtocreate"_tr());
    auto handler = mc->getServerNetworkHandler();
    if (!handler) return base::OperateResult::error("manager.error.failedtocreate"_tr());

    fix::CFSPFixManager::getInstance().createSpMutex = true;
    this->mSimPlayer                                 = SimulatedPlayer::create(
        this->mSaveData.name,
        {0, 0, 0},
        {0, 0, 0},
        {0, 0},
        false,
        0,
        handler,
        this->mSaveData.xuid,
        this->mSaveData.uniqueId.has_value()
            ? std::optional<ActorUniqueID>(ActorUniqueID(this->mSaveData.uniqueId.value()))
            : std::nullopt
    );
    fix::CFSPFixManager::getInstance().createSpMutex = false;

    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->loadSpNbt();
    this->mSimPlayer->teleport(this->mSimPlayer->getFeetPos(), this->mSimPlayer->getDimensionId());
    this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition = this->mSimPlayer->getFeetPos();
    this->mSimPlayer->mPlayerRespawnPoint->mDimension      = this->mSimPlayer->getDimensionId();
    this->lookAt(this->mSimPlayer->getHeadPos() + this->mSaveData.lookAtOffSet);
    if (player.has_value())
        this->mSaveData.lastSpawnerUuid = player.value() ? player.value()->getUuid().asString() : "";
    this->mSaveData.isOnline = true;
    this->mShouldSave        = true;
    this->mSimPlayer->setPlayerGameType(this->mSaveData.mGameType);
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::despawn() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    this->stop();
    this->mSaveData.isOnline = false;
    this->mShouldSave        = true;
    this->save();

    auto& removingRecord = fix::CFSPFixManager::getInstance().mRemovingRecord;
    auto  currentTick    = this->mSimPlayer->getLevel().getCurrentTick();
    if (currentTick.tickID - removingRecord.mLastTick.tickID > 2) {
        removingRecord.mRemovingSpList.clear();
    }
    removingRecord.mLastTick = currentTick;
    removingRecord.mRemovingSpList.emplace(this->mSimPlayer->mName);


    this->mSimPlayer->disconnect();
    this->mSimPlayer->remove();
    this->mSimPlayer->setGameTestHelper(nullptr);
    this->mSimPlayer = nullptr;
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::respawn() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isAlive()) return base::OperateResult::error("manager.fail.spIsAlive"_tr());
    if (!this->mSimPlayer->simulateRespawn()) return base::OperateResult::error("manager.fail.failToRespawn"_tr());
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::info() {
    using ll::i18n_literals::operator""_tr;
    std::string res  = "\n  " + "manager.info.spname"_tr() + this->mSaveData.name + "\n  ";
    res             += "manager.info.spOwner"_tr() + base::utils::tryGetPlayerName(this->mSaveData.ownerUuid) + "\n  ";
    res += "manager.info.spStatus"_tr()
         + (this->mSimPlayer ? this->mSimPlayer->isAlive() ? "base.spstatus.alive"_tr() : "base.spstatus.dead"_tr()
                             : "base.spstatus.offline"_tr())
         + "\n  ";
    if (this->mSimPlayer) {
        res += "manager.info.spPos"_tr() + base::utils::getDimName(this->mSimPlayer->getDimensionId()) + " "
             + this->mSimPlayer->getPosition().toString() + "\n  ";
        res += "manager.info.spRespawnpos"_tr()
             + base::utils::getDimName(this->mSimPlayer->mPlayerRespawnPoint->mDimension->mValue) + " "
             + this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        res += "manager.info.spGamemode"_tr() + base::utils::getGameModeStr((int)this->mSimPlayer->getPlayerGameType())
             + "\n  ";
        res += "manager.info.spHealth"_tr() + std::to_string(mSimPlayer->getHealth()) + " / "
             + std::to_string(this->mSimPlayer->getMaxHealth()) + "\n  ";
        res += "manager.info.spIsFree"_tr() + (this->isFree() ? "base.yesOrNo.yes"_tr() : "base.yesOrNo.no"_tr());
    }
    return base::OperateResult::success(res);
}

base::OperateResult SimPlayer::lookAt(Vec3 const& pos) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    this->mSaveData.lookAtOffSet = pos - this->mSimPlayer->getHeadPos();
    this->mSimPlayer->simulateLookAt(pos, sim::LookDuration::Instant);
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::lookAt(Direction direction) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());

    Vec3 offSet;
    switch (direction) {
    case Direction::North:
        offSet = {0, 0, -1};
        break;
    case Direction::South:
        offSet = {0, 0, 1};
        break;
    case Direction::West:
        offSet = {-1, 0, 0};
        break;
    case Direction::East:
        offSet = {1, 0, 0};
        break;
    case Direction::Up:
        offSet = {0, 1, 0};
        break;
    case Direction::Down:
        offSet = {0, -1, 0};
        break;
    default:
        return base::OperateResult::error("manager.fail.invalidDirection"_tr());
    }

    Vec3 pos = this->mSimPlayer->getHeadPos() + offSet;
    return this->lookAt(pos);
}
} // namespace coral_fans::cfsp::simulated_player