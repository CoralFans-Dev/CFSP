#include "mc/math/Vec2.h"
#include "cfsp/base/Macros.h"
#include "cfsp/simplayer/luaapi/Utils.h"
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata vec2 begin

LUAAPI(vec2_new) {
    LUA_ARG_COUNT_CHECK_C(2)
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    lua_settop(L, 0);
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    rot->x    = x;
    rot->y    = y;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newLowest) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::LOWEST;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newMax) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::MAX;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newMin) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::MIN;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newNegUnitX) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::NEG_UNIT_X;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newNegUnitY) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::NEG_UNIT_Y;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newOne) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::ONE;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newUnitX) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::UNIT_X;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newUnitY) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::UNIT_Y;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_newZero) {
    LUA_ARG_COUNT_CHECK_C(0)
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = Vec2::ZERO;
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

static const luaL_Reg lua_reg_vec2_c[11] = {
    {"new",         lua_api_vec2_new        },
    {"newLowest",   lua_api_vec2_newLowest  },
    {"newMax",      lua_api_vec2_newMax     },
    {"newMin",      lua_api_vec2_newMin     },
    {"newNegUnitX", lua_api_vec2_newNegUnitX},
    {"newNegUnitY", lua_api_vec2_newNegUnitY},
    {"newOne",      lua_api_vec2_newOne     },
    {"newUnitX",    lua_api_vec2_newUnitX   },
    {"newUnitY",    lua_api_vec2_newUnitY   },
    {"newZero",     lua_api_vec2_newZero    },
    {NULL,          NULL                    }
};

LUAAPI(vec2_get) {
    LUA_ARG_COUNT_CHECK_M(0)
    Vec2* rot = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushnumber(L, rot->x);
    lua_pushnumber(L, rot->y);
    return 2;
}

LUAAPI(vec2_meta_tostring) {
    LUA_ARG_COUNT_CHECK_M(0)
    Vec2* rot = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot != nullptr, 1, "invalid userdata");
    lua_settop(L, 0);
    lua_pushstring(L, rot->toString().c_str());
    return 1;
}

LUAAPI(vec2_meta_add) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec2* rot1 = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot1 != nullptr, 1, "invalid userdata");
    Vec2* rot2 = (Vec2*)luaL_checkudata(L, 2, "vec2_mt");
    luaL_argcheck(L, rot2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = (*rot1) + (*rot2);
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_meta_sub) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec2* rot1 = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot1 != nullptr, 1, "invalid userdata");
    Vec2* rot2 = (Vec2*)luaL_checkudata(L, 2, "vec2_mt");
    luaL_argcheck(L, rot2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    Vec2* rot = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *rot      = (*rot1) - (*rot2);
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(vec2_meta_eq) {
    LUA_ARG_COUNT_CHECK_M(1)
    Vec2* rot1 = (Vec2*)luaL_checkudata(L, 1, "vec2_mt");
    luaL_argcheck(L, rot1 != nullptr, 1, "invalid userdata");
    Vec2* rot2 = (Vec2*)luaL_checkudata(L, 2, "vec2_mt");
    luaL_argcheck(L, rot2 != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    lua_pushboolean(L, ((*rot1) == (*rot2)));
    return 1;
}

static const luaL_Reg lua_reg_vec2_m[6] = {
    {"get",        lua_api_vec2_get          },
    {"__tostring", lua_api_vec2_meta_tostring},
    {"__add",      lua_api_vec2_meta_add     },
    {"__sub",      lua_api_vec2_meta_sub     },
    {"__eq",       lua_api_vec2_meta_eq      },
    {NULL,         NULL                      }
};

LUAAPI(open_vec2) {
    luaL_newmetatable(L, "vec2_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_vec2_m, 0);
    luaL_newlib(L, lua_reg_vec2_c);
    return 1;
}

// userdata vec2 end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp