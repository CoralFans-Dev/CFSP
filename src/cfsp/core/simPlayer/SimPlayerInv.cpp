#include "SimPlayer.h"
#include "ll/api/i18n/I18n.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h "
#include "mc/nbt/CompoundTag.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include <exception>


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

base::OperateResult SimPlayer::drop() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->drop(this->mSimPlayer->getSelectedItem(), 0))
        this->mSimPlayer->setSelectedItem(ItemStack::EMPTY_ITEM());
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::dropInv() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    auto& inv  = *this->mSimPlayer->mInventory->mInventory;
    int   sel  = this->mSimPlayer->getSelectedItemSlot();
    int   size = inv.getContainerSize();
    for (int i = 0; i < size; ++i) {
        inv.swapSlots(i, sel);
        if (this->mSimPlayer->drop(this->mSimPlayer->getSelectedItem(), 0))
            this->mSimPlayer->setSelectedItem(ItemStack::EMPTY_ITEM());
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::swap(Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (!player) throw std::invalid_argument("Player is null");
    std::vector<std::string> invKeys = {"Armor", "EnderChestInventory", "Inventory", "Mainhand", "Offhand"};
    auto                     spTag   = std::make_unique<CompoundTag>();
    if (!this->mSimPlayer->save(*spTag)) return base::OperateResult::error("manager.fail.failToSave"_tr());
    auto pTag = std::make_unique<CompoundTag>();
    if (!player->save(*pTag)) return base::OperateResult::error("manager.fail.failToSave"_tr());
    for (auto& key : invKeys) {
        auto spInvNode = spTag->mTags.extract(key);
        auto pInvNode  = pTag->mTags.extract(key);
        spTag->mTags.insert(std::move(pInvNode));
        pTag->mTags.insert(std::move(spInvNode));
    }
    try {
        DefaultDataLoadHelper helper;
        player->load(*pTag, helper);
        player->sendInventory(true);
        this->mSimPlayer->load(*spTag, helper);
    } catch (std::exception ex) {
        return base::OperateResult::error(std::string("Error: ") + ex.what());
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player