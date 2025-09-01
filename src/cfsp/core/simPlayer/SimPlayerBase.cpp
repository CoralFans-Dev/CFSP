#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/actor/provider/ActorEquipment.h"

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
} // namespace coral_fans::cfsp::simulated_player