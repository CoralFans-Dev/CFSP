#include "CFSPFixManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"


namespace coral_fans::cfsp::fix {
LL_TYPE_INSTANCE_HOOK(
    CFSPHandItemFixHook1,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::inventoryChanged,
    void,
    Container&       container,
    int              slot,
    ItemStack const& oldItem,
    ItemStack const& newItem,
    bool             forceBalanced
) {
    origin(container, slot, oldItem, newItem, forceBalanced);
    if (this->isSimulatedPlayer()) {
        if (slot == 0 && oldItem.getTypeName() != newItem.getTypeName()) {
            MobEquipmentPacket(
                this->getRuntimeID(),
                newItem,
                0,
                0,
                mInventory->mSelectedContainerId
            )
                .sendToClients(); // fix::更新主手
        }
    }
}

LL_TYPE_INSTANCE_HOOK(
    CFSPHandItemFixHook2,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setOffhandSlot,
    void,
    ItemStack const& item
) {
    if (this->isSimulatedPlayer()) {
        if (this->getOffhandSlot().getTypeName() != item.getTypeName())
            MobEquipmentPacket(this->getRuntimeID(), item, 1, 0,
                               ContainerID::Offhand).sendToClients(); // fix::更新副手
    }
    origin(item);
}

void CFSPFixManager::handItemFix() {
    CFSPHandItemFixHook1::hook();
    CFSPHandItemFixHook2::hook();
}
} // namespace coral_fans::cfsp::fix