#include "mc/world/level/BlockPos.h"
#include "cfsp/base/Macros.h"
#include "cfsp/simplayer/luaapi/Vec3.h"
#include "ll/api/base/StdInt.h"
#include "mc/math/Vec3.h"
#include "mc/world/level/BlockSource.h"
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blockpos begin

LUAAPI(blockpos_new) {
    LUA_ARG_COUNT_CHECK_C(3)
    int x = static_cast<int>(luaL_checkinteger(L, 1));
    int y = static_cast<int>(luaL_checkinteger(L, 2));
    int z = static_cast<int>(luaL_checkinteger(L, 3));
    lua_settop(L, 0);
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    bp->x        = x;
    bp->y        = y;
    bp->z        = z;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_newMax) {
    LUA_ARG_COUNT_CHECK_C(0)
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = BlockPos::MAX;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_newMin) {
    LUA_ARG_COUNT_CHECK_C(0)
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = BlockPos::MIN;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_newOne) {
    LUA_ARG_COUNT_CHECK_C(0)
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = BlockPos::ONE;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_newZero) {
    LUA_ARG_COUNT_CHECK_C(0)
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = BlockPos::ZERO;
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

const luaL_Reg lua_reg_blockpos_c[6] = {
    {"new",     lua_api_blockpos_new    },
    {"newMax",  lua_api_blockpos_newMax },
    {"newMin",  lua_api_blockpos_newMin },
    {"newOne",  lua_api_blockpos_newOne },
    {"newZero", lua_api_blockpos_newZero},
    {NULL,      NULL                    }
};

LUAAPI(blockpos_get) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushinteger(L, bp->x);
    lua_pushinteger(L, bp->y);
    lua_pushinteger(L, bp->z);
    return 3;
}

LUAAPI(blockpos_bottomCenter) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = bp->bottomCenter();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(blockpos_center) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = bp->center();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(blockpos_toVec3) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3(*bp);
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(blockpos_neighbor) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    uchar facing = static_cast<uchar>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    BlockPos* ret_bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *ret_bp          = bp->neighbor(facing);
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_meta_tostring) {
    LUA_ARG_COUNT_CHECK_M(0)
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, bp->toString().c_str());
    return 1;
}

LUAAPI(blockpos_meta_add) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockPos* bp1 = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp1 != nullptr, 1, "invalid userdata");
    BlockPos* bp2 = (BlockPos*)luaL_checkudata(L, 2, "blockpos_mt");
    luaL_argcheck(L, bp2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = (*bp1) + (*bp2);
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_meta_sub) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockPos* bp1 = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp1 != nullptr, 1, "invalid userdata");
    BlockPos* bp2 = (BlockPos*)luaL_checkudata(L, 2, "blockpos_mt");
    luaL_argcheck(L, bp2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    BlockPos* bp = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *bp          = (*bp1) - (*bp2);
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(blockpos_meta_eq) {
    LUA_ARG_COUNT_CHECK_M(1)
    BlockPos* bp1 = (BlockPos*)luaL_checkudata(L, 1, "blockpos_mt");
    luaL_argcheck(L, bp1 != nullptr, 1, "invalid userdata");
    BlockPos* bp2 = (BlockPos*)luaL_checkudata(L, 2, "blockpos_mt");
    luaL_argcheck(L, bp2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, ((*bp1) == (*bp2)));
    return 1;
}

const luaL_Reg lua_reg_blockpos_m[10] = {
    {"get",          lua_api_blockpos_get          },
    {"bottomCenter", lua_api_blockpos_bottomCenter },
    {"center",       lua_api_blockpos_center       },
    {"toVec3",       lua_api_blockpos_toVec3       },
    {"neighbor",     lua_api_blockpos_neighbor     },
    {"__tostring",   lua_api_blockpos_meta_tostring},
    {"__add",        lua_api_blockpos_meta_add     },
    {"__sub",        lua_api_blockpos_meta_sub     },
    {"__eq",         lua_api_blockpos_meta_eq      },
    {NULL,           NULL                          }
};

LUAAPI(open_blockpos) {
    luaL_newmetatable(L, "blockpos_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_blockpos_m, 0);
    luaL_newlib(L, lua_reg_blockpos_c);
    return 1;
}

// userdata blockpos end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp