#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata vec2 begin

int lua_api_vec2_new(lua_State*);

int lua_api_vec2_newLowest(lua_State*);

int lua_api_vec2_newMax(lua_State*);

int lua_api_vec2_newMin(lua_State*);

int lua_api_vec2_newNegUnitX(lua_State*);

int lua_api_vec2_newNegUnitY(lua_State*);

int lua_api_vec2_newOne(lua_State*);

int lua_api_vec2_newUnitX(lua_State*);

int lua_api_vec2_newUnitY(lua_State*);

int lua_api_vec2_newZero(lua_State*);

extern const luaL_Reg lua_reg_vec2_c[11];

int lua_api_vec2_get(lua_State*);

int lua_api_vec2_meta_tostring(lua_State*);

int lua_api_vec2_meta_add(lua_State*);

int lua_api_vec2_meta_sub(lua_State*);

int lua_api_vec2_meta_eq(lua_State*);

extern const luaL_Reg lua_reg_vec2_m[6];

int lua_api_open_vec2(lua_State*);

// userdata vec2 end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp