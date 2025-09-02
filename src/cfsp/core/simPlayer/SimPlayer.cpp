#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Schedule.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/entity/components_json_legacy/NavigationComponent.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/server/sim/sim.h"
#include "mc/world/Minecraft.h"
#include "mc/world/actor/ai/navigation/PathNavigation.h"
#include "mc/world/actor/provider/MobMovement.h"
#include <optional>

namespace coral_fans::cfsp::simulated_player {
void SimPlayer::cancelTask() { base::Schedule::getInstance().getSchedule()->cancel(this->mTaskid); }

void SimPlayer::cancelScript() { base::Schedule::getInstance().getSchedule()->cancel(this->mScriptid); }

base::OperateResult SimPlayer::stop() {
    using ll::i18n_literals::operator""_tr;
    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mTaskid)) this->cancelTask();
    this->mTaskid = 0;
    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mScriptid)) this->cancelScript();
    this->mScriptid = 0;

    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    ;

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
    if (this->mSimPlayer->isAlive()) {
        this->mSimPlayer->releaseUsingItem();
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

std::shared_ptr<SimPlayer>
SimPlayer::create(const Player* player, std::string const& spname, Vec3 const& pos, DimensionType dim) {
    auto uuid = player->getUuid().asString();
    auto mc   = ll::service::getMinecraft();
    if (!mc) return nullptr;
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return nullptr;
    auto  xuid      = "-" + std::to_string(std::hash<std::string>()(spname));
    auto* simPlayer = SimulatedPlayer::create(spname, pos, dim, serverNetworkHandler, xuid, std::nullopt);
    if (!simPlayer) return nullptr;

    simPlayer->mPlayerRespawnPoint->mPlayerPosition = pos;
    simPlayer->mPlayerRespawnPoint->mDimension      = dim;

    SimPlayerSaveData saveData;
    saveData.name            = spname;
    saveData.uniqueId        = simPlayer->getOrCreateUniqueID().rawID;
    saveData.xuid            = xuid;
    saveData.ownerUuid       = uuid;
    saveData.lastSpawnerUuid = uuid;

    return std::make_shared<simulated_player::SimPlayer>(saveData, simPlayer);
}

base::OperateResult SimPlayer::spawn(std::optional<const Player*> player, bool lockUniqueId) {
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
        lockUniqueId ? std::optional<ActorUniqueID>(ActorUniqueID(this->mSaveData.uniqueId)) : std::nullopt
    );
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.failedtocreate"_tr());
    this->loadSpNbt();
    this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition = this->mSimPlayer->getFeetPos();
    this->mSimPlayer->mPlayerRespawnPoint->mDimension      = this->mSimPlayer->getDimensionId();
    this->lookAt(this->mSimPlayer->getEyePos() + this->mSaveData.lookAtOffSet);
    if (player.has_value()) this->mSaveData.lastSpawnerUuid = player.value()->getUuid().asString();
    this->mSaveData.isOnline = true;
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::despawn() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    this->stop();
    this->mSaveData.isOnline = false;
    this->save();
    this->mSimPlayer->disconnect();
    this->mSimPlayer->remove();
    this->mSimPlayer->setGameTestHelper(nullptr);
    this->mSimPlayer = nullptr;
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::respawn() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isAlive()) return base::OperateResult::error("manager.fail.SpIsAlive"_tr());
    auto& spawnPos                              = this->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition;
    this->mSimPlayer->mRespawnPositionCandidate = {spawnPos->x + 0.5f, spawnPos->y + 1.62001f, spawnPos->z + 0.5f};
    this->mSimPlayer->mRespawnReady             = true;
    this->mSimPlayer->mRespawningFromTheEnd     = false;
    this->mSimPlayer->respawn();
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::lookAt(Vec3 const& pos) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    this->mSimPlayer->mLookAtIntent->mType = std::get<::sim::ContinuousLookAtPositionIntent>(
        sim::lookAt(*this->mSimPlayer, glm::vec3(pos.x, pos.y, pos.z), ::sim::LookDuration::UntilMove).mType.get()
    );
    this->mSaveData.lookAtOffSet = pos - this->mSimPlayer->getEyePos();
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player