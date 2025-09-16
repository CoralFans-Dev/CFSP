#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Schedule.h"
#include "cfsp/base/Utils.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/entity/components_json_legacy/NavigationComponent.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/server/sim/sim.h"
#include "mc/world/Minecraft.h"
#include "mc/world/actor/ai/navigation/PathNavigation.h"
#include "mc/world/actor/provider/ActorAttribute.h"
#include "mc/world/actor/provider/MobMovement.h"
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

    // simPlayer->simulateStopInteracting();
    this->mSimPlayer->deleteContainerManager();

    // simPlayer->simulateStopMoving();
    // from lse
    auto& type = this->mSimPlayer->mSimulatedMovement->mType.get();
    if (std::holds_alternative<sim::MoveInDirectionIntent>(type)
        || std::holds_alternative<sim::MoveToPositionIntent>(type)) {
        MobMovement::setLocalMoveVelocity(this->mSimPlayer->getEntityContext(), Vec3::ZERO());
    } else if (std::holds_alternative<sim::NavigateToPositionsIntent>(type)
               || std::holds_alternative<sim::NavigateToEntityIntent>(type)) {
        MobMovement::setLocalMoveVelocity(this->mSimPlayer->getEntityContext(), Vec3::ZERO());
        auto component = this->mSimPlayer->getEntityContext().tryGetComponent<NavigationComponent>();
        if (component) {
            component->mNavigation->stop(component, *this->mSimPlayer);
        }
    }

    // simPlayer->simulateStopUsingItem();
    if (this->mSimPlayer->isAlive()) [[likely]]
        this->mSimPlayer->releaseUsingItem();

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
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return nullptr;
    auto  xuid      = "-" + std::to_string(std::hash<std::string>()(spname));
    auto* simPlayer = SimulatedPlayer::create(spname, pos, dim, serverNetworkHandler, xuid, std::nullopt);
    if (!simPlayer) [[unlikely]]
        return nullptr;

    simPlayer->mPlayerRespawnPoint->mPlayerPosition = pos;
    simPlayer->mPlayerRespawnPoint->mDimension      = dim;

    SimPlayerSaveData saveData;
    saveData.name = spname;
    if (lockUniqueId) saveData.uniqueId = simPlayer->getOrCreateUniqueID().rawID;
    saveData.xuid            = xuid;
    saveData.ownerUuid       = uuid;
    saveData.lastSpawnerUuid = uuid;

    auto cfsp = std::make_shared<simulated_player::SimPlayer>(saveData, simPlayer);

    cfsp->lookAt(
        player ? simPlayer->getEyePos() + Vec3::directionFromRotation(player->getRotation()) : simPlayer->getEyePos()
    );

    return cfsp;
}

base::OperateResult SimPlayer::spawn(std::optional<const Player*> player) {
    using ll::i18n_literals::operator""_tr;
    auto mc = ll::service::getMinecraft();
    if (!mc) return base::OperateResult::error("manager.error.failedtocreate"_tr());
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return base::OperateResult::error("manager.error.failedtocreate"_tr());

    this->mSimPlayer = SimulatedPlayer::create(
        this->mSaveData.name,
        {0, 0, 0},
        0,
        serverNetworkHandler,
        this->mSaveData.xuid,
        this->mSaveData.uniqueId.has_value()
            ? std::optional<ActorUniqueID>(ActorUniqueID(this->mSaveData.uniqueId.value()))
            : std::nullopt
    );
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->loadSpNbt();
    this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition = this->mSimPlayer->getFeetPos();
    this->mSimPlayer->mPlayerRespawnPoint->mDimension      = this->mSimPlayer->getDimensionId();
    this->lookAt(this->mSimPlayer->getEyePos() + this->mSaveData.lookAtOffSet);
    if (player.has_value())
        this->mSaveData.lastSpawnerUuid = player.value() ? player.value()->getUuid().asString() : "";
    this->mSaveData.isOnline = true;
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
    auto& spawnPos                              = this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition;
    this->mSimPlayer->mRespawnPositionCandidate = {spawnPos->x + 0.5f, spawnPos->y + 1.62001f, spawnPos->z + 0.5f};
    this->mSimPlayer->mRespawnReady             = true;
    this->mSimPlayer->mRespawningFromTheEnd     = false;
    this->mSimPlayer->respawn();
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::info() {
    using ll::i18n_literals::operator""_tr;
    std::string res  = "\n  " + "manager.info.spname"_tr() + this->mSaveData.name + "\n  ";
    res             += "manager.info.spOwner"_tr() + base::utils::tryGetPlayerName(this->mSaveData.ownerUuid) + "\n  ";
    res             += "manager.info.spStatus"_tr()
         + (this->mSimPlayer ? this->mSimPlayer->isAlive() ? "base.spstatus.alive"_tr() : "base.spstatus.dead"_tr()
                             : "base.spstatus.offline"_tr())
         + "\n  ";
    if (this->mSimPlayer) {
        res += "manager.info.spPos"_tr() + base::utils::getDimName(this->mSimPlayer->getDimensionId()) + " "
             + this->mSimPlayer->getPosition().toJsonString() + "\n  ";
        res += "manager.info.spRespawnpos"_tr()
             + base::utils::getDimName(this->mSimPlayer->mPlayerRespawnPoint->mDimension->id) + " "
             + this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        res += "manager.info.spGamemode"_tr() + base::utils::getGameModeStr((int)this->mSimPlayer->getPlayerGameType())
             + "\n  ";
        res += "manager.info.spHealth"_tr()
             + std::to_string(ActorAttribute::getHealth(this->mSimPlayer->getEntityContext())) + " / "
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
    this->mSimPlayer->mLookAtIntent->mType = std::get<::sim::ContinuousLookAtPositionIntent>(
        sim::lookAt(*this->mSimPlayer, glm::vec3(pos.x, pos.y, pos.z), ::sim::LookDuration::UntilMove).mType.get()
    );
    this->mSaveData.lookAtOffSet = pos - this->mSimPlayer->getEyePos();
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player