#include "SimPlayer.h"
#include "SimPlayerSaveData.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/Minecraft.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include <boost/iostreams/device/mapped_file.hpp>

namespace coral_fans::cfsp::simulated_player {
SimPlayer::SimPlayer(SimPlayerSaveData saveData, SimulatedPlayer* sp) {
    this->mSaveData  = saveData;
    this->mSimPlayer = sp;
    if (!sp) return;
    auto ec = this->mSimPlayer->getEnderChestContainer();
    if (!ec.has_value() || ec->isEmpty()) this->mIsEnderContainerEmpty = true;
    else this->mIsEnderContainerEmpty = false;
    this->mIsInventoryEmpty = this->mSimPlayer->mInventory->mInventory->isEmpty();
    this->mIsOffhandEmpty   = this->mSimPlayer->getOffhandSlot() == ItemStack::EMPTY_ITEM();
    this->mIsEquipmentEmpty = ActorEquipment::getArmorContainer(this->mSimPlayer->getEntityContext()).isEmpty();
}

inline std::string SimPlayer::getName() { return this->mSaveData.name; }

inline std::string SimPlayer::getXuid() { return this->mSaveData.xuid; }

inline bool SimPlayer::isOnline() { return this->mSimPlayer != nullptr; }

base::OperateResult SimPlayer::hasPermission(Player* player, SimPlayerPermission permission) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return base::OperateResult::success();
    auto it = this->mSaveData.permission.find(uuid);
    if (it != this->mSaveData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission)
        return base::OperateResult::success();
    it = this->mSaveData.permission.find("");
    if (it != this->mSaveData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission)
        return base::OperateResult::success();
    return base::OperateResult::error("manager.fail.permissionDenied"_tr());
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