#include "mc/deps/core/math/Vec3.h"
#include "cfsp/base/Macros.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include <string>


extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata vec3 begin

LUAAPI(vec3_new) {
    LUA_ARG_COUNT_CHECK_C(3)
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_checknumber(L, 3));
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    pos->x    = x;
    pos->y    = y;
    pos->z    = z;
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newHalf) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::HALF();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newMax) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::MAX();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newMin) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::MIN();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newNegUnitX) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::NEG_UNIT_X();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newNegUnitY) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::NEG_UNIT_Y();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newNegUnitZ) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::NEG_UNIT_Z();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newOne) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::ONE();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newTwo) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::TWO();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newUnitX) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::UNIT_X();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newUnitY) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::UNIT_Y();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newUnitZ) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::UNIT_Z();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newZero) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::ZERO();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_newFromRotation) {
    LUA_ARG_COUNT_CHECK_C(1)
    Vec2* rot = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = Vec3::directionFromRotation(*rot);
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

const luaL_Reg lua_reg_vec3_c[15] = {
    {"new",             lua_api_vec3_new            },
    {"newHalf",         lua_api_vec3_newHalf        },
    {"newMax",          lua_api_vec3_newMax         },
    {"newMin",          lua_api_vec3_newMin         },
    {"newNegUnitX",     lua_api_vec3_newNegUnitX    },
    {"newNegUnitY",     lua_api_vec3_newNegUnitY    },
    {"newNegUnitZ",     lua_api_vec3_newNegUnitZ    },
    {"newOne",          lua_api_vec3_newOne         },
    {"newTwo",          lua_api_vec3_newTwo         },
    {"newUnitX",        lua_api_vec3_newUnitX       },
    {"newUnitY",        lua_api_vec3_newUnitY       },
    {"newUnitZ",        lua_api_vec3_newUnitZ       },
    {"newZero",         lua_api_vec3_newZero        },
    {"newFromRotation", lua_api_vec3_newFromRotation},
    {NULL,              NULL                        }
};

LUAAPI(vec3_get) {
    LUA_ARG_COUNT_CHECK_M(0)
    Vec3* pos = (Vec3*)luaL_checkudata(L, 1, "vec3_mt");
    luaL_argcheck(L, pos != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushnumber(L, pos->x);
    lua_pushnumber(L, pos->y);
    lua_pushnumber(L, pos->z);
    return 3;
}

LUAAPI(vec3_meta_tostring) {
    LUA_ARG_COUNT_CHECK_M(0)
    Vec3* pos = (Vec3*)luaL_checkudata(L, 1, "vec3_mt");
    luaL_argcheck(L, pos != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, pos->toString().c_str());
    return 1;
}

LUAAPI(vec3_meta_add) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec3* pos1 = (Vec3*)luaL_checkudata(L, 1, "vec3_mt");
    luaL_argcheck(L, pos1 != nullptr, 1, "invalid userdata");
    Vec3* pos2 = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = (*pos1) + (*pos2);
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_meta_sub) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec3* pos1 = (Vec3*)luaL_checkudata(L, 1, "vec3_mt");
    luaL_argcheck(L, pos1 != nullptr, 1, "invalid userdata");
    Vec3* pos2 = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = (*pos1) - (*pos2);
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(vec3_meta_eq) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec3* pos1 = (Vec3*)luaL_checkudata(L, 1, "vec3_mt");
    luaL_argcheck(L, pos1 != nullptr, 1, "invalid userdata");
    Vec3* pos2 = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, ((*pos1) == (*pos2)));
    return 1;
}

const luaL_Reg lua_reg_vec3_m[6] = {
    {"get",        lua_api_vec3_get          },
    {"__tostring", lua_api_vec3_meta_tostring},
    {"__add",      lua_api_vec3_meta_add     },
    {"__sub",      lua_api_vec3_meta_sub     },
    {"__eq",       lua_api_vec3_meta_eq      },
    {NULL,         NULL                      }
};

LUAAPI(open_vec3) {
    luaL_newmetatable(L, "vec3_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_vec3_m, 0);
    luaL_newlib(L, lua_reg_vec3_c);
    return 1;
}

// userdata vec3 end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp