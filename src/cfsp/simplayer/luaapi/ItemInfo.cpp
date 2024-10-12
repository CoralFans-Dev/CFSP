#include "cfsp/simplayer/luaapi/ItemInfo.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/Utils.h"
#include "cfsp/simplayer/luaapi/Utils.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/Tag.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include <memory>
#include <string>
#include <utility>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata iteminfo begin

LUAAPI(iteminfo_getName) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*info)->name.c_str());
    return 1;
}

LUAAPI(iteminfo_getType) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*info)->type.c_str());
    return 1;
}

LUAAPI(iteminfo_getId) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->id);
    return 1;
}

LUAAPI(iteminfo_getCount) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->count);
    return 1;
}

LUAAPI(iteminfo_getAux) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->aux);
    return 1;
}

LUAAPI(iteminfo_getDamage) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->damage);
    return 1;
}

LUAAPI(iteminfo_getAttackDamage) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->attackDamage);
    return 1;
}

LUAAPI(iteminfo_getMaxDamage) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->maxDamage);
    return 1;
}

LUAAPI(iteminfo_getMaxStackSize) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*info)->maxStackSize);
    return 1;
}

LUAAPI(iteminfo_getLore) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    int idx = 1;
    lua_newtable(L);
    for (const auto& item : (*info)->lore) {
        lua_pushstring(L, item.c_str());
        lua_seti(L, -2, idx);
        ++idx;
    }
    return 1;
}

LUAAPI(iteminfo_isArmorItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isArmorItem);
    return 1;
}

LUAAPI(iteminfo_isBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isBlock);
    return 1;
}

LUAAPI(iteminfo_isDamageableItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isDamageableItem);
    return 1;
}

LUAAPI(iteminfo_isDamaged) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isDamaged);
    return 1;
}

LUAAPI(iteminfo_isEnchanted) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isEnchanted);
    return 1;
}

LUAAPI(iteminfo_isEnchantingBook) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isEnchantingBook);
    return 1;
}

LUAAPI(iteminfo_isFireResistant) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isFireResistant);
    return 1;
}

LUAAPI(iteminfo_isFullStack) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isFullStack);
    return 1;
}

LUAAPI(iteminfo_isGlint) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isGlint);
    return 1;
}

LUAAPI(iteminfo_isHorseArmorItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isHorseArmorItem);
    return 1;
}

LUAAPI(iteminfo_isLiquidClipItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isLiquidClipItem);
    return 1;
}

LUAAPI(iteminfo_isMusicDiscItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isMusicDiscItem);
    return 1;
}

LUAAPI(iteminfo_isOffhandItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isOffhandItem);
    return 1;
}

LUAAPI(iteminfo_isPotionItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isPotionItem);
    return 1;
}

LUAAPI(iteminfo_isStackable) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isStackable);
    return 1;
}

LUAAPI(iteminfo_isWearableItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*info)->isWearableItem);
    return 1;
}

LUAAPI(iteminfo_getNbtTable) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    compoundTagToLuaTable(L, *((*info)->tag));
    return 1;
}

LUAAPI(iteminfo_getSnbt) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*info)->tag->toSnbt(SnbtFormat::Minimize).c_str());
    return 1;
}

LUAAPI(iteminfo_getSnbtWithPath) {
    LUA_ARG_COUNT_CHECK_M(1)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    std::string path = luaL_checkstring(L, 2);
    lua_settop(L, 0);
    const auto& rst = utils::getNbtFromTag(*((*info)->tag), path);
    lua_pushstring(L, rst.first.c_str());
    lua_pushboolean(L, rst.second);
    return 2;
}

LUAAPI(iteminfo_meta_eq) {
    LUA_ARG_COUNT_CHECK_M(1)
    ItemInfo** info1 = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info1 != nullptr) && ((*info1) != nullptr), 1, "invalid userdata");
    ItemInfo** info2 = (ItemInfo**)luaL_checkudata(L, 2, "iteminfo_mt");
    luaL_argcheck(L, (info2 != nullptr) && ((*info2) != nullptr), 2, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, ((**info1) == (**info2)));
    return 1;
}

LUAAPI(iteminfo_meta_gc) {
    LUA_ARG_COUNT_CHECK_M(0)
    ItemInfo** info = (ItemInfo**)luaL_checkudata(L, 1, "iteminfo_mt");
    luaL_argcheck(L, (info != nullptr) && ((*info) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    delete *info;
    return 0;
}

const luaL_Reg lua_reg_iteminfo_m[32] = {
    {"getName",          lua_api_iteminfo_getName         },
    {"getType",          lua_api_iteminfo_getType         },
    {"getId",            lua_api_iteminfo_getId           },
    {"getCount",         lua_api_iteminfo_getCount        },
    {"getAux",           lua_api_iteminfo_getAux          },
    {"getDamage",        lua_api_iteminfo_getDamage       },
    {"getAttackDamage",  lua_api_iteminfo_getAttackDamage },
    {"getMaxDamage",     lua_api_iteminfo_getMaxDamage    },
    {"getMaxStackSize",  lua_api_iteminfo_getMaxStackSize },
    {"getLore",          lua_api_iteminfo_getLore         },
    {"isArmorItem",      lua_api_iteminfo_isArmorItem     },
    {"isBlock",          lua_api_iteminfo_isBlock         },
    {"isDamageableItem", lua_api_iteminfo_isDamageableItem},
    {"isDamaged",        lua_api_iteminfo_isDamaged       },
    {"isEnchanted",      lua_api_iteminfo_isEnchanted     },
    {"isEnchantingBook", lua_api_iteminfo_isEnchantingBook},
    {"isFireResistant",  lua_api_iteminfo_isFireResistant },
    {"isFullStack",      lua_api_iteminfo_isFullStack     },
    {"isGlint",          lua_api_iteminfo_isGlint         },
    {"isHorseArmorItem", lua_api_iteminfo_isHorseArmorItem},
    {"isLiquidClipItem", lua_api_iteminfo_isLiquidClipItem},
    {"isMusicDiscItem",  lua_api_iteminfo_isMusicDiscItem },
    {"isOffhandItem",    lua_api_iteminfo_isOffhandItem   },
    {"isPotionItem",     lua_api_iteminfo_isPotionItem    },
    {"isStackable",      lua_api_iteminfo_isStackable     },
    {"isWearableItem",   lua_api_iteminfo_isWearableItem  },
    {"getNbtTable",      lua_api_iteminfo_getNbtTable     },
    {"getSnbt",          lua_api_iteminfo_getSnbt         },
    {"getSnbtWithPath",  lua_api_iteminfo_getSnbtWithPath },
    {"__eq",             lua_api_iteminfo_meta_eq         },
    {"__gc",             lua_api_iteminfo_meta_gc         },
    {NULL,               NULL                             }
};

LUAAPI(open_iteminfo) {
    luaL_newmetatable(L, "iteminfo_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_iteminfo_m, 0);
    luaL_newlib(L, lua_reg_null_c);
    return 1;
}

// userdata iteminfo end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp