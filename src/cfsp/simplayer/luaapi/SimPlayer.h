#pragma once

#include "cfsp/base/Macros.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata simplayer begin

int lua_api_simplayer_getName(lua_State*);

int lua_api_simplayer_getXuid(lua_State*);

int lua_api_simplayer_getStatus(lua_State*);

int lua_api_simplayer_getPos(lua_State*);

int lua_api_simplayer_getFeetPos(lua_State*);

int lua_api_simplayer_getStandingOn(lua_State*);

int lua_api_simplayer_getRot(lua_State*);

int lua_api_simplayer_getHealth(lua_State*);

int lua_api_simplayer_getHunger(lua_State*);

int lua_api_simplayer_sneaking(lua_State*);

int lua_api_simplayer_swimming(lua_State*);

int lua_api_simplayer_attack(lua_State*);

int lua_api_simplayer_chat(lua_State*);

int lua_api_simplayer_destroy(lua_State*);

int lua_api_simplayer_dropSelectedItem(lua_State*);

int lua_api_simplayer_dropInv(lua_State*);

int lua_api_simplayer_runCmd(lua_State*);

int lua_api_simplayer_getBlockPosFromView(lua_State*);

int lua_api_simplayer_searchInInvWithId(lua_State*);

int lua_api_simplayer_searchInInvWithName(lua_State*);

int lua_api_simplayer_selectSlot(lua_State*);

int lua_api_simplayer_select(lua_State*);

int lua_api_simplayer_getItemFromInv(lua_State*);

int lua_api_simplayer_interact(lua_State*);

int lua_api_simplayer_jump(lua_State*);

int lua_api_simplayer_useItem(lua_State*);

int lua_api_simplayer_startBuild(lua_State*);

int lua_api_simplayer_lookAt(lua_State*);

int lua_api_simplayer_moveTo(lua_State*);

int lua_api_simplayer_navigateTo(lua_State*);

int lua_api_simplayer_isTaskFree(lua_State*);

int lua_api_simplayer_stopAction(lua_State*);

int lua_api_simplayer_meta_gc(lua_State*);

extern const luaL_Reg lua_reg_simplayer_m[34];

int lua_api_open_simplayer(lua_State*);

// userdata simplayer end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp