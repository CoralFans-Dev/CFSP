#include "mc/world/level/BlockSource.h"
#include "cfsp/base/Macros.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/dimension/Dimension.h"

#include "cfsp/simplayer/luaapi/BlockInfo.h"
#include "cfsp/simplayer/luaapi/Utils.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blocksource begin

LUAAPI(blocksource_getBlockInfo) {
    LUA_ARG_COUNT_CHECK_M(1)
    int* dimid = (int*)luaL_checkudata(L, 1, "blocksource_mt");
    luaL_argcheck(L, dimid != nullptr, 1, "invalid userdata");
    BlockPos* bp = (BlockPos*)luaL_checkudata(L, 2, "blockpos_mt");
    luaL_argcheck(L, bp != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        lua_pushnil(L);
        lua_pushboolean(L, false);
        return 2;
    }
    const auto& block = level->getDimension(*dimid)->getBlockSourceFromMainChunkSource().getBlock(*bp);
    BlockInfo** binfo = (BlockInfo**)lua_newuserdata(L, sizeof(BlockInfo*));
    *binfo            = new BlockInfo(*bp, *dimid, block);
    luaL_setmetatable(L, "blockinfo_mt");
    lua_pushboolean(L, true);
    return 2;
}

const luaL_Reg lua_reg_blocksource_m[2] = {
    {"getBlockInfo", lua_api_blocksource_getBlockInfo},
    {NULL,           NULL                            }
};

LUAAPI(open_blocksource) {
    luaL_newmetatable(L, "blocksource_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_blocksource_m, 0);
    luaL_newlib(L, lua_reg_null_c);
    return 1;
}

// userdata blocksource end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp