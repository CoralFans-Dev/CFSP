#include "SimPlayer.h"
#include "cfsp/core/manager/SimPlayerManager.h"
#include "ll/api/memory/Hook.h"
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

LL_TYPE_INSTANCE_HOOK(
    CFSPSimPlayerSaveHelperHook1,
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
    auto cfsp = manager::SimPlayerManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value()) {
        if (slot == 0 && oldItem.getTypeName() != newItem.getTypeName()) {
            MobEquipmentPacket(
                getRuntimeID(),
                newItem,
                0,
                0,
                mInventory->mSelectedContainerId
            )
                .sendToClients(); // fix::更新主手
        }
        if (cfsp.value()->mIsEnderContainerEmpty) {
            if (newItem == ItemStack::EMPTY_ITEM()) {
                if (!cfsp.value()->mIsInventoryEmpty && this->mInventory->mInventory->isEmpty()) {
                    cfsp.value()->mIsInventoryEmpty = true;
                    if (cfsp.value()->mIsEquipmentEmpty && cfsp.value()->mIsOffhandEmpty) {
                        cfsp.value()->mSaveData.isEmptyInv = true;
                        cfsp.value()->mShouldSave          = true;
                    }
                }
            } else if (cfsp.value()->mIsInventoryEmpty) {
                cfsp.value()->mIsInventoryEmpty = false;
                if (cfsp.value()->mIsEquipmentEmpty && cfsp.value()->mIsOffhandEmpty) {
                    cfsp.value()->mSaveData.isEmptyInv = false;
                    cfsp.value()->mShouldSave          = true;
                }
            }
        }
    }
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSimPlayerSaveHelperHook2,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setOffhandSlot,
    void,
    ItemStack const& item
) {
    auto cfsp = manager::SimPlayerManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value()) {
        if (this->getOffhandSlot().getTypeName() != item.getTypeName())
            MobEquipmentPacket(this->getRuntimeID(), item, 1, 0,
                               ContainerID::Offhand).sendToClients(); // fix::更新副手
        if (cfsp.value()->mIsEnderContainerEmpty) {
            if (item == ItemStack::EMPTY_ITEM()) {
                if (!cfsp.value()->mIsOffhandEmpty) {
                    cfsp.value()->mIsOffhandEmpty = true;
                    if (cfsp.value()->mIsInventoryEmpty && cfsp.value()->mIsEquipmentEmpty) {
                        cfsp.value()->mSaveData.isEmptyInv = true;
                        cfsp.value()->mShouldSave          = true;
                    }
                }
            } else if (cfsp.value()->mIsOffhandEmpty) {
                cfsp.value()->mIsOffhandEmpty = false;
                if (cfsp.value()->mIsInventoryEmpty && cfsp.value()->mIsEquipmentEmpty) {
                    cfsp.value()->mSaveData.isEmptyInv = false;
                    cfsp.value()->mShouldSave          = true;
                }
            }
        }
    }
    origin(item);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSimPlayerSaveHelperHook3,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setEquippedSlot,
    void,
    ::SharedTypes::Legacy::EquipmentSlot slot,
    ::ItemStack const&                   item
) {
    auto cfsp = manager::SimPlayerManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value()) {
        if (cfsp.value()->mIsEnderContainerEmpty) {
            if (item == ItemStack::EMPTY_ITEM()) {
                if (!cfsp.value()->mIsEquipmentEmpty
                    && ActorEquipment::getArmorContainer(this->getEntityContext()).isEmpty()) {
                    cfsp.value()->mIsEquipmentEmpty = true;
                    if (cfsp.value()->mIsInventoryEmpty && cfsp.value()->mIsOffhandEmpty) {
                        cfsp.value()->mSaveData.isEmptyInv = true;
                        cfsp.value()->mShouldSave          = true;
                    }
                }
            } else if (cfsp.value()->mIsEquipmentEmpty) {
                cfsp.value()->mIsEquipmentEmpty = false;
                if (cfsp.value()->mIsInventoryEmpty && cfsp.value()->mIsOffhandEmpty) {
                    cfsp.value()->mSaveData.isEmptyInv = false;
                    cfsp.value()->mShouldSave          = true;
                }
            }
        }
    }
    origin(slot, item);
}

void SimPlayer::simPlayerInvHook() {
    CFSPSimPlayerSaveHelperHook1::hook();
    CFSPSimPlayerSaveHelperHook2::hook();
    CFSPSimPlayerSaveHelperHook3::hook();
}
} // namespace coral_fans::cfsp::simulated_player