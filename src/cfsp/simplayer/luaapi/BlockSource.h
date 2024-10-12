#pragma once

#include "cfsp/base/Macros.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blocksource begin

int lua_api_blocksource_getBlockInfo(lua_State*);

extern const luaL_Reg lua_reg_blocksource_m[2];

int lua_api_open_blocksource(lua_State*);

// userdata blocksource end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp