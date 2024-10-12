#pragma once

#include "cfsp/base/Macros.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata vec3 begin

int lua_api_vec3_new(lua_State*);

int lua_api_vec3_newHalf(lua_State*);

int lua_api_vec3_newMax(lua_State*);

int lua_api_vec3_newMin(lua_State*);

int lua_api_vec3_newNegUnitX(lua_State*);

int lua_api_vec3_newNegUnitY(lua_State*);

int lua_api_vec3_newNegUnitZ(lua_State*);

int lua_api_vec3_newOne(lua_State*);

int lua_api_vec3_newTwo(lua_State*);

int lua_api_vec3_newUnitX(lua_State*);

int lua_api_vec3_newUnitY(lua_State*);

int lua_api_vec3_newUnitZ(lua_State*);

int lua_api_vec3_newZero(lua_State*);

extern const luaL_Reg lua_reg_vec3_c[14];

int lua_api_vec3_get(lua_State*);

int lua_api_vec3_meta_tostring(lua_State*);

int lua_api_vec3_meta_add(lua_State*);

int lua_api_vec3_meta_sub(lua_State*);

int lua_api_vec3_meta_eq(lua_State*);

extern const luaL_Reg lua_reg_vec3_m[6];

int lua_api_open_vec3(lua_State*);

// userdata vec3 end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp