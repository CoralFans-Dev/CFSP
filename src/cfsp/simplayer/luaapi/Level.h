#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata level begin

int lua_api_level_getDayTime(lua_State*);

int lua_api_level_getGameTime(lua_State*);

int lua_api_level_getDay(lua_State*);

int lua_api_level_getMspt(lua_State*);

extern const luaL_Reg lua_reg_level_m[5];

int lua_api_open_level(lua_State*);

// userdata level end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp