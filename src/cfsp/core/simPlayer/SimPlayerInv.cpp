#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
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

base::OperateResult SimPlayer ::invInfo() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    std::string res = '\n' + "manager.info.spname"_tr() + this->mSaveData.name + "\n  " + "manager.info.spOffhand"_tr();
    auto&       _itemstack  = this->mSimPlayer->getOffhandSlot();
    res                    += "§6" + _itemstack.getName() + "§2(" + std::to_string(_itemstack.mCount) + ")§r";
    res                    += "  " + "manager.info.spEquip"_tr();
    for (auto& itemstack : ActorEquipment::getArmorContainer(this->mSimPlayer->getEntityContext()))
        res += "§6" + itemstack.getName() + "§2(" + std::to_string(itemstack.mCount) + ")§r, ";
    res += "  " + "manager.info.spInv"_tr();
    for (auto& itemstack : *this->mSimPlayer->mInventory->mInventory->mItems)
        res += "§6" + itemstack.getName() + "§2(" + std::to_string(itemstack.mCount) + ")§r, ";
    return base::OperateResult::success(res);
}

base::OperateResult SimPlayer::drop() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (this->mSimPlayer->drop(this->mSimPlayer->getSelectedItem(), 0)) [[likely]]
        this->mSimPlayer->setSelectedItem(ItemStack::EMPTY_ITEM());
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::dropInv() {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    auto& inv  = *this->mSimPlayer->mInventory->mInventory;
    int   sel  = this->mSimPlayer->getSelectedItemSlot();
    int   size = inv.getContainerSize();
    for (int i = 0; i < size; ++i) {
        inv.swapSlots(i, sel);
        if (this->mSimPlayer->drop(this->mSimPlayer->getSelectedItem(), 0)) [[likely]]
            this->mSimPlayer->setSelectedItem(ItemStack::EMPTY_ITEM());
    }
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::swap(Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (!player) [[unlikely]]
        return base::OperateResult::error("manager.fail.playerIsNull"_tr());
    std::vector<std::string> invKeys = {"Armor", "EnderChestInventory", "Inventory", "Mainhand", "Offhand"};
    auto                     spTag   = std::make_unique<CompoundTag>();
    if (!this->mSimPlayer->save(*spTag)) [[unlikely]]
        return base::OperateResult::error("manager.fail.failToSave"_tr());
    auto pTag = std::make_unique<CompoundTag>();
    if (!player->save(*pTag)) [[unlikely]]
        return base::OperateResult::error("manager.fail.failToSave"_tr());
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

base::OperateResult SimPlayer::select(int id) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    auto& inv  = *this->mSimPlayer->mInventory->mInventory;
    int   size = inv.getContainerSize();
    for (int i = 0; i < size; i++)
        if (inv.getItem(i).getId() == id) {
            inv.swapSlots(i, this->mSimPlayer->getSelectedItemSlot());
            MobEquipmentPacket(
                this->mSimPlayer->getRuntimeID(),
                this->mSimPlayer->getSelectedItem(),
                0,
                0,
                this->mSimPlayer->mInventory->mSelectedContainerId
            )
                .sendToClients(); // fix::更新主手
            return base::OperateResult::success("manager.success.operate"_tr());
        }
    return base::OperateResult::error("manager.fail.selectNoFound"_tr());
}
} // namespace coral_fans::cfsp::simulated_player