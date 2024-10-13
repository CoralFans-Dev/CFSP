#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blockpos begin

int lua_api_blockpos_new(lua_State*);

int lua_api_blockpos_newMax(lua_State*);

int lua_api_blockpos_newMin(lua_State*);

int lua_api_blockpos_newOne(lua_State*);

int lua_api_blockpos_newZero(lua_State*);

extern const luaL_Reg lua_reg_blockpos_c[6];

int lua_api_blockpos_get(lua_State*);

int lua_api_blockpos_bottomCenter(lua_State*);

int lua_api_blockpos_center(lua_State*);

int lua_api_blockpos_toVec3(lua_State*);

int lua_api_blockpos_neighbor(lua_State*);

int lua_api_blockpos_meta_tostring(lua_State*);

int lua_api_blockpos_meta_add(lua_State*);

int lua_api_blockpos_meta_sub(lua_State*);

int lua_api_blockpos_meta_eq(lua_State*);

extern const luaL_Reg lua_reg_blockpos_m[10];

int lua_api_open_blockpos(lua_State*);

// userdata blockpos end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp