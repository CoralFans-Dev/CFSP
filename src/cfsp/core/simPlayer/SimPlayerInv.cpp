#include "SimPlayer.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/actor/provider/ActorEquipment.h"


namespace coral_fans::cfsp::simulated_player {
bool SimPlayer::isEmptyInv() {
    if (!this->mSimPlayer) return false;
    if (this->mSimPlayer->mInventory->mInventory->isEmpty()
        || this->mSimPlayer->getOffhandSlot() == ItemStack::EMPTY_ITEM()
        || ActorEquipment::getArmorContainer(this->mSimPlayer->getEntityContext()).isEmpty())
        return false;
    auto ec = this->mSimPlayer->getEnderChestContainer();
    if (ec.has_value() && ec->isEmpty()) return false;
    return true;
}

} // namespace coral_fans::cfsp::simulated_player