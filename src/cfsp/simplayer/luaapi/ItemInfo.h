#pragma once

#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/Tag.h"
#include "mc/world/item/registry/ItemStack.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include <memory>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata iteminfo begin

class ItemInfo {
public:
    std::string                  name;
    std::string                  type;
    int                          id;
    int                          count;
    int                          aux;
    int                          damage;
    int                          attackDamage;
    int                          maxDamage;
    int                          maxStackSize;
    std::vector<std::string>     lore;
    bool                         isArmorItem;
    bool                         isBlock;
    bool                         isDamageableItem;
    bool                         isDamaged;
    bool                         isEnchanted;
    bool                         isEnchantingBook;
    bool                         isFireResistant;
    bool                         isFullStack;
    bool                         isGlint;
    bool                         isHorseArmorItem;
    bool                         isLiquidClipItem;
    bool                         isMusicDiscItem;
    bool                         isOffhandItem;
    bool                         isPotionItem;
    bool                         isStackable;
    bool                         isWearableItem;
    std::unique_ptr<CompoundTag> tag;

public:
    ItemInfo(ItemStack const& item)
    : name(item.getCustomName().empty() ? item.getName() : item.getCustomName()),
      type(item.getTypeName()),
      id(item.getId()),
      count(item.mCount),
      aux(item.getAuxValue()),
      damage(item.getDamageValue()),
      attackDamage(item.getAttackDamage()),
      maxDamage(item.getMaxDamage()),
      maxStackSize(item.getMaxStackSize()),
      lore(item.getCustomLore()),
      isArmorItem(item.isArmorItem()),
      isBlock(item.isBlock()),
      isDamageableItem(item.isDamageableItem()),
      isDamaged(item.isDamaged()),
      isEnchanted(item.isEnchanted()),
      isEnchantingBook(item.isEnchantingBook()),
      isFireResistant(item.isFireResistant()),
      isFullStack(item.isFullStack()),
      isGlint(item.isGlint()),
      isHorseArmorItem(item.isHorseArmorItem()),
      isLiquidClipItem(item.isLiquidClipItem()),
      isMusicDiscItem(item.isMusicDiscItem()),
      isOffhandItem(item.isOffhandItem()),
      isPotionItem(item.isPotionItem()),
      isStackable(item.isStackable()),
      isWearableItem(item.isHumanoidWearableItem()),
      tag(item.save()) {}
    bool operator==(const ItemInfo& rt) const {
        return name == rt.name && type == rt.type && id == rt.id && count == rt.count && aux == rt.aux
            && damage == rt.damage && attackDamage == rt.attackDamage && maxDamage == rt.maxDamage
            && maxStackSize == rt.maxStackSize && lore == rt.lore && isArmorItem == rt.isArmorItem
            && isBlock == rt.isBlock && isDamageableItem == rt.isDamageableItem && isDamaged == rt.isDamaged
            && isEnchanted == rt.isEnchanted && isEnchantingBook == rt.isEnchantingBook
            && isFireResistant == rt.isFireResistant && isFullStack == rt.isFullStack && isGlint == rt.isGlint
            && isHorseArmorItem == rt.isHorseArmorItem && isLiquidClipItem == rt.isLiquidClipItem
            && isMusicDiscItem == rt.isMusicDiscItem && isOffhandItem == rt.isOffhandItem
            && isPotionItem == rt.isPotionItem && isStackable == rt.isStackable && isWearableItem == rt.isWearableItem
            && (*tag) == (*rt.tag);
    }
};

int lua_api_iteminfo_getName(lua_State*);

int lua_api_iteminfo_getType(lua_State*);

int lua_api_iteminfo_getId(lua_State*);

int lua_api_iteminfo_getCount(lua_State*);

int lua_api_iteminfo_getAux(lua_State*);

int lua_api_iteminfo_getDamage(lua_State*);

int lua_api_iteminfo_getAttackDamage(lua_State*);

int lua_api_iteminfo_getMaxDamage(lua_State*);

int lua_api_iteminfo_getMaxStackSize(lua_State*);

int lua_api_iteminfo_getLore(lua_State*);

int lua_api_iteminfo_isArmorItem(lua_State*);

int lua_api_iteminfo_isBlock(lua_State*);

int lua_api_iteminfo_isDamageableItem(lua_State*);

int lua_api_iteminfo_isDamaged(lua_State*);

int lua_api_iteminfo_isEnchanted(lua_State*);

int lua_api_iteminfo_isEnchantingBook(lua_State*);

int lua_api_iteminfo_isFireResistant(lua_State*);

int lua_api_iteminfo_isFullStack(lua_State*);

int lua_api_iteminfo_isGlint(lua_State*);

int lua_api_iteminfo_isHorseArmorItem(lua_State*);

int lua_api_iteminfo_isLiquidClipItem(lua_State*);

int lua_api_iteminfo_isMusicDiscItem(lua_State*);

int lua_api_iteminfo_isOffhandItem(lua_State*);

int lua_api_iteminfo_isPotionItem(lua_State*);

int lua_api_iteminfo_isStackable(lua_State*);

int lua_api_iteminfo_isWearableItem(lua_State*);

int lua_api_iteminfo_getNbtTable(lua_State*);

int lua_api_iteminfo_getSnbt(lua_State*);

int lua_api_iteminfo_getSnbtWithPath(lua_State*);

int lua_api_iteminfo_meta_eq(lua_State*);

int lua_api_iteminfo_meta_gc(lua_State*);

extern const luaL_Reg lua_reg_iteminfo_m[32];

int lua_api_open_iteminfo(lua_State*);

// userdata iteminfo end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp