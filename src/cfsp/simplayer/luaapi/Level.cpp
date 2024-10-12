#include "mc/world/level/Level.h"
#include "cfsp/base/Macros.h"
#include "cfsp/simplayer/luaapi/Utils.h"
#include "ll/api/service/Bedrock.h"
#include "mc/util/ProfilerLite.h"
#include "mc/world/level/Tick.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata level begin

LUAAPI(level_getDayTime) {
    LUA_ARG_COUNT_CHECK_M(0)
    luaL_checkudata(L, 1, "level_mt");
    lua_settop(L, 0);
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        lua_pushinteger(L, -1);
        return 1;
    }
    lua_pushinteger(L, level->getTime() % 24000);
    return 1;
}

LUAAPI(level_getGameTime) {
    LUA_ARG_COUNT_CHECK_M(0)
    luaL_checkudata(L, 1, "level_mt");
    lua_settop(L, 0);
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        lua_pushinteger(L, -1);
        return 1;
    }
    lua_pushinteger(L, level->getCurrentTick().t);
    return 1;
}

LUAAPI(level_getDay) {
    LUA_ARG_COUNT_CHECK_M(0)
    luaL_checkudata(L, 1, "level_mt");
    lua_settop(L, 0);
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        lua_pushinteger(L, -1);
        return 1;
    }
    lua_pushinteger(L, level->getTime() / 24000);
    return 1;
}

LUAAPI(level_getMspt) {
    LUA_ARG_COUNT_CHECK_M(0)
    luaL_checkudata(L, 1, "level_mt");
    lua_settop(L, 0);
    lua_pushnumber(L, static_cast<double>(ProfilerLite::gProfilerLiteInstance.getServerTickTime().count() / 1000000.0));
    return 1;
}

const luaL_Reg lua_reg_level_m[5] = {
    {"getDayTime",  lua_api_level_getDayTime },
    {"getGameTime", lua_api_level_getGameTime},
    {"getDay",      lua_api_level_getDay     },
    {"getMspt",     lua_api_level_getMspt    },
    {NULL,          NULL                     }
};

LUAAPI(open_level) {
    luaL_newmetatable(L, "level_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_level_m, 0);
    luaL_newlib(L, lua_reg_null_c);
    return 1;
}

// userdata level end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp