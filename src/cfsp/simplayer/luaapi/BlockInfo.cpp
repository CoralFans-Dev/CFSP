#include "cfsp/simplayer/luaapi/BlockInfo.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/Utils.h"
#include "cfsp/simplayer/luaapi/Utils.h"
#include "mc/nbt/Tag.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blockinfo begin

LUAAPI(blockinfo_getName) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*binfo)->name.c_str());
    return 1;
}

LUAAPI(blockinfo_getType) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*binfo)->type.c_str());
    return 1;
}

LUAAPI(blockinfo_getId) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*binfo)->id);
    return 1;
}

LUAAPI(blockinfo_getPos) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = (*binfo)->pos;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockinfo_getDim) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*binfo)->dim);
    return 1;
}

LUAAPI(blockinfo_getVariant) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, (*binfo)->variant);
    return 1;
}

LUAAPI(blockinfo_getTranslucency) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushnumber(L, (*binfo)->translucency);
    return 1;
}

LUAAPI(blockinfo_getThickness) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushnumber(L, (*binfo)->thickness);
    return 1;
}

LUAAPI(blockinfo_isAir) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isAir);
    return 1;
}

LUAAPI(blockinfo_isBounceBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isBounceBlock);
    return 1;
}

LUAAPI(blockinfo_isButtonBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isButtonBlock);
    return 1;
}

LUAAPI(blockinfo_isCropBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isCropBlock);
    return 1;
}

LUAAPI(blockinfo_isDoorBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isDoorBlock);
    return 1;
}

LUAAPI(blockinfo_isFallingBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isFallingBlock);
    return 1;
}

LUAAPI(blockinfo_isFenceBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isFenceBlock);
    return 1;
}

LUAAPI(blockinfo_isFenceGateBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isFenceGateBlock);
    return 1;
}

LUAAPI(blockinfo_isSlabBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isSlabBlock);
    return 1;
}

LUAAPI(blockinfo_isStemBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isStemBlock);
    return 1;
}

LUAAPI(blockinfo_isThinFenceBlock) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isThinFenceBlock);
    return 1;
}

LUAAPI(blockinfo_isUnbreakable) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, (*binfo)->isUnbreakable);
    return 1;
}

LUAAPI(blockinfo_getNbtTable) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    compoundTagToLuaTable(L, *((*binfo)->tag));
    return 1;
}

LUAAPI(blockinfo_getSnbt) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, (*binfo)->tag->toSnbt(SnbtFormat::Minimize).c_str());
    return 1;
}

LUAAPI(blockinfo_getSnbtWithPath) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    std::string path = luaL_checkstring(L, 2);
    lua_settop(L, 0);
    const auto& rst = utils::getNbtFromTag(*((*binfo)->tag), path);
    lua_pushstring(L, rst.first.c_str());
    lua_pushboolean(L, rst.second);
    return 2;
}

LUAAPI(blockinfo_meta_eq) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockInfo** binfo1 = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo1 != nullptr) && ((*binfo1) != nullptr), 1, "invalid userdata");
    BlockInfo** binfo2 = (BlockInfo**)luaL_checkudata(L, 2, "blockinfo_mt");
    luaL_argcheck(L, (binfo2 != nullptr) && ((*binfo2) != nullptr), 2, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, ((**binfo1) == (**binfo2)));
    return 1;
}

LUAAPI(blockinfo_meta_gc) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockInfo** binfo = (BlockInfo**)luaL_checkudata(L, 1, "blockinfo_mt");
    luaL_argcheck(L, (binfo != nullptr) && ((*binfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    delete *binfo;
    return 0;
}

const luaL_Reg lua_reg_blockinfo_m[27] = {
    {"getName",          lua_api_blockinfo_getName         },
    {"getType",          lua_api_blockinfo_getType         },
    {"getId",            lua_api_blockinfo_getId           },
    {"getPos",           lua_api_blockinfo_getPos          },
    {"getDim",           lua_api_blockinfo_getDim          },
    {"getVariant",       lua_api_blockinfo_getVariant      },
    {"getTranslucency",  lua_api_blockinfo_getTranslucency },
    {"getThickness",     lua_api_blockinfo_getThickness    },
    {"isAir",            lua_api_blockinfo_isAir           },
    {"isBounceBlock",    lua_api_blockinfo_isBounceBlock   },
    {"isButtonBlock",    lua_api_blockinfo_isButtonBlock   },
    {"isCropBlock",      lua_api_blockinfo_isCropBlock     },
    {"isDoorBlock",      lua_api_blockinfo_isDoorBlock     },
    {"isFallingBlock",   lua_api_blockinfo_isFallingBlock  },
    {"isFenceBlock",     lua_api_blockinfo_isFenceBlock    },
    {"isFenceGateBlock", lua_api_blockinfo_isFenceGateBlock},
    {"isSlabBlock",      lua_api_blockinfo_isSlabBlock     },
    {"isStemBlock",      lua_api_blockinfo_isStemBlock     },
    {"isThinFenceBlock", lua_api_blockinfo_isThinFenceBlock},
    {"isUnbreakable",    lua_api_blockinfo_isUnbreakable   },
    {"getNbtTable",      lua_api_blockinfo_getNbtTable     },
    {"getSnbt",          lua_api_blockinfo_getSnbt         },
    {"getSnbtWithPath",  lua_api_blockinfo_getSnbtWithPath },
    {"__eq",             lua_api_blockinfo_meta_eq         },
    {"__gc",             lua_api_blockinfo_meta_gc         },
    {NULL,               NULL                              }
};

LUAAPI(open_blockinfo) {
    luaL_newmetatable(L, "blockinfo_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_blockinfo_m, 0);
    luaL_newlib(L, lua_reg_null_c);
    return 1;
}

// userdata blockinfo end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp