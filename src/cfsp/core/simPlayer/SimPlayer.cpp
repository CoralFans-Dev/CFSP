#include "SimPlayer.h"
#include "cfsp/base/Schedule.h"
#include "ll/api/service/Bedrock.h"
#include "mc/entity/components_json_legacy/NavigationComponent.h"
#include "mc/world/Minecraft.h"
#include "mc/world/actor/ai/navigation/PathNavigation.h"
#include "mc/world/actor/provider/MobMovement.h"


namespace coral_fans::cfsp::simulated_player {
void SimPlayer::cancelTask() { base::Schedule::getInstance().getSchedule()->cancel(this->mTaskid); }

void SimPlayer::cancelScript() { base::Schedule::getInstance().getSchedule()->cancel(this->mScriptid); }

void SimPlayer::stopAction() {
    if (!this->mSimPlayer) throw std::invalid_argument("SimPlayer is null");

    // simPlayer->simulateStopBuild();
    // int8& isbuilding = ((int8*)&simPlayer->mBuildIntention)[1];
    // if (isbuilding) {
    //     simPlayer->mGameMode->stopBuildBlock();
    //     isbuilding = 0;
    // }

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
    if (this->mSimPlayer) {
        if (this->mSimPlayer->isAlive()) {
            this->mSimPlayer->releaseUsingItem();
        }
    }

    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mTaskid)) this->cancelTask();
    this->mTaskid = 0;
}

void SimPlayer::stop() {
    stopAction();
    if (base::Schedule::getInstance().getSchedule()->isRunning(this->mScriptid)) this->cancelScript();
    this->mScriptid = 0;
}

std::shared_ptr<SimPlayer> SimPlayer::create(
    const Player*      player,
    std::string const& spname,
    Vec3 const&        pos,
    DimensionType      dim,
    bool               isLockUniqueId
) {
    auto uuid = player->getUuid().asString();
    auto mc   = ll::service::getMinecraft();
    if (!mc) return nullptr;
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return nullptr;
    auto  xuid      = "-" + std::to_string(std::hash<std::string>()(spname));
    auto* simPlayer = SimulatedPlayer::create(
        spname,
        pos,
        pos,
        {0, 0},
        isLockUniqueId,
        dim,
        serverNetworkHandler,
        xuid,
        std::nullopt
    );
    if (!simPlayer) return nullptr;

    simPlayer->mPlayerRespawnPoint->mPlayerPosition = pos;
    simPlayer->mPlayerRespawnPoint->mDimension      = dim;

    SimPlayerSaveData saveData;
    saveData.name = spname;
    if (isLockUniqueId) saveData.uniqueId = simPlayer->getOrCreateUniqueID().rawID;
    else saveData.uniqueId = std::nullopt;
    saveData.xuid            = xuid;
    saveData.ownerUuid       = uuid;
    saveData.lastSpawnerUuid = uuid;

    return std::make_shared<simulated_player::SimPlayer>(saveData, simPlayer);
}

bool SimPlayer::spawn(const Player* player) {
    auto mc = ll::service::getMinecraft();
    if (!mc) return false;
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return false;
    auto* simPlayer = SimulatedPlayer::create(
        this->mSaveData.name,
        {0, 0, 0},
        {0, 0, 0},
        {0, 0},
        this->mSaveData.uniqueId.has_value(),
        0,
        serverNetworkHandler,
        this->mSaveData.xuid,
        this->mSaveData.uniqueId.has_value()
            ? std::optional<ActorUniqueID>(ActorUniqueID(this->mSaveData.uniqueId.value()))
            : std::nullopt
    );
    if (!simPlayer) return false;

    simPlayer->mPlayerRespawnPoint->mPlayerPosition = simPlayer->getFeetPos();
    simPlayer->mPlayerRespawnPoint->mDimension      = simPlayer->getDimensionId();
    this->mSimPlayer                                = simPlayer;
    this->mSaveData.lastSpawnerUuid                 = player->getUuid().asString();

    return true;
}

void SimPlayer::despawn() {
    this->mSimPlayer->disconnect();
    this->mSimPlayer->remove();
    this->mSimPlayer->setGameTestHelper(nullptr);
}
} // namespace coral_fans::cfsp::simulated_player