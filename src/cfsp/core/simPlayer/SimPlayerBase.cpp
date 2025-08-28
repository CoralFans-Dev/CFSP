#include "SimPlayer.h"
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

bool SimPlayer::hasPermission(Player* player) {
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return true;
    if (this->mSaveData.permission.contains(uuid)) return true;
    return false;
}
} // namespace coral_fans::cfsp::simulated_player