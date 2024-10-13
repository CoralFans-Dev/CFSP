#pragma once

#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/ListTag.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

extern const luaL_Reg lua_reg_null_c[1];

void compoundTagToLuaTable(lua_State* L, CompoundTagVariant tag);

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp