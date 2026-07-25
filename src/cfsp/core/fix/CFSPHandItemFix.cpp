#include "CFSPFixManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#endif


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
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(container, slot, oldItem, newItem, forceBalanced);
#endif
    origin(container, slot, oldItem, newItem, forceBalanced);
    if (this->isSimulatedPlayer()) {
        if (slot == 0 && oldItem.getTypeName() != newItem.getTypeName()) {
            MobEquipmentPacketPayload payload(this->getRuntimeID(), newItem, 0, 0, mInventory->mSelectedContainerId);
            MobEquipmentPacket(payload).sendToClients(); // fix::更新主手
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
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(item);
#endif
    if (this->isSimulatedPlayer()) {
        if (this->getOffhandSlot().getTypeName() != item.getTypeName()) {
            MobEquipmentPacketPayload payload(this->getRuntimeID(), item, 1, 0, ContainerID::Offhand);
            MobEquipmentPacket(payload).sendToClients(); // fix::更新副手
        }
    }
    origin(item);
}

void CFSPFixManager::handItemFix(bool enable) {
    if (enable) {
        CFSPHandItemFixHook1::hook();
        CFSPHandItemFixHook2::hook();
    } else {
        CFSPHandItemFixHook1::unhook();
        CFSPHandItemFixHook2::unhook();
    }
}
} // namespace coral_fans::cfsp::fix