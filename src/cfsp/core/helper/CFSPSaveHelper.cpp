#include "CFSPHelperManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/server/commands/StopCommand.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/storage/LevelStorage.h"

namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook,
    ::ll::memory::HookPriority::Normal,
    LevelStorage,
    &LevelStorage::save,
    void,
    ::Player& player
) {
    origin(player);
    if (auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(&player); cfsp.has_value()) cfsp.value()->save();
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook1,
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
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value()) {
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
        if (cfsp.value()->mIsEnderContainerEmpty) {
            if (newItem == ItemStack::EMPTY_ITEM() && !cfsp.value()->mIsInventoryEmpty) {
                this->mInventory->mInventory->mItems.get()[slot] = ItemStack::EMPTY_ITEM();
                if (this->mInventory->mInventory->isEmpty()) {
                    cfsp.value()->mIsInventoryEmpty = true;
                    if (cfsp.value()->mIsEquipmentEmpty && cfsp.value()->mIsOffhandEmpty) {
                        cfsp.value()->mSaveData.isEmptyInv = true;
                        cfsp.value()->mShouldSave          = true;
                    }
                }
                this->mInventory->mInventory->mItems.get()[slot] = oldItem;

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
    CFSPSaveHelperHook2,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setOffhandSlot,
    void,
    ItemStack const& item
) {
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
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
    CFSPSaveHelperHook3,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setEquippedSlot,
    void,
    ::SharedTypes::Legacy::EquipmentSlot slot,
    ::ItemStack const&                   item
) {
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
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

LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook4,
    ll::memory::HookPriority::Normal,
    StopCommand,
    &StopCommand::$execute,
    void,
    CommandOrigin const& arg1,
    CommandOutput&       arg2
) {
    manager::CFSPManager::getInstance().saveSps();
    origin(arg1, arg2);
}

void CFSPHelperManager::saveHelperHook() {
    CFSPSaveHelperHook ::hook();
    CFSPSaveHelperHook1::hook();
    CFSPSaveHelperHook2::hook();
    CFSPSaveHelperHook3::hook();
    CFSPSaveHelperHook4::hook();
}
} // namespace coral_fans::cfsp::helper