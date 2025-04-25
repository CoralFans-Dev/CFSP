#include "cfsp/base/Macros.h"
#include "cfsp/simplayer/CFSP.h"
#include "mc/deps/core/math/Vec2.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include <string>

#include "cfsp/simplayer/luaapi/ItemInfo.h"
#include "cfsp/simplayer/luaapi/Utils.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata simplayer begin

LUAAPI(simplayer_getName) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushstring(L, (**spinfo)->getName().c_str());
    return 1;
}

LUAAPI(simplayer_getXuid) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushstring(L, (**spinfo)->getXuid().c_str());
    return 1;
}

LUAAPI(simplayer_getStatus) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->getStatus());
    return 1;
}

LUAAPI(simplayer_getPos) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = (**spinfo)->getPos();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(simplayer_getFeetPos) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    Vec3* pos = (Vec3*)lua_newuserdata(L, sizeof(Vec3));
    *pos      = (**spinfo)->getFeetPos();
    luaL_setmetatable(L, "vec3_mt");
    return 1;
}

LUAAPI(simplayer_getStandingOn) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    BlockPos* pos = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *pos          = (**spinfo)->getStandingOn();
    luaL_setmetatable(L, "blockpos_mt");
    return 1;
}

LUAAPI(simplayer_getRot) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    Vec2* pos = (Vec2*)lua_newuserdata(L, sizeof(Vec2));
    *pos      = (**spinfo)->getRot();
    luaL_setmetatable(L, "vec2_mt");
    return 1;
}

LUAAPI(simplayer_getHealth) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->getHealth());
    return 1;
}

LUAAPI(simplayer_getHunger) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushnumber(L, (**spinfo)->getHunger());
    return 1;
}

LUAAPI(simplayer_sneaking) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    luaL_argcheck(
        L,
        lua_isboolean(L, 2),
        2,
        std::string{"boolean expected, got " + std::string{lua_typename(L, lua_type(L, 2))}}.c_str()
    );
    lua_pushboolean(L, (**spinfo)->sneaking(lua_toboolean(L, 2)));
    return 1;
}

LUAAPI(simplayer_swimming) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    luaL_argcheck(
        L,
        lua_isboolean(L, 2),
        2,
        std::string{"boolean expected, got " + std::string{lua_typename(L, lua_type(L, 2))}}.c_str()
    );
    (**spinfo)->swimming(lua_toboolean(L, 2));
    return 0;
}

LUAAPI(simplayer_attack) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->attack());
    return 1;
}

LUAAPI(simplayer_chat) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    std::string msg = luaL_checkstring(L, 2);
    lua_settop(L, 0);
    (**spinfo)->chat(msg);
    return 0;
}

LUAAPI(simplayer_destroy) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->destroy());
    return 1;
}

LUAAPI(simplayer_dropSelectedItem) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->dropSelectedItem());
    return 1;
}

LUAAPI(simplayer_dropInv) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->dropInv());
    return 1;
}

LUAAPI(simplayer_runCmd) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    std::string cmd = luaL_checkstring(L, 2);
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->runCmd(cmd));
    return 1;
}

LUAAPI(simplayer_getBlockPosFromView) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    const auto& rst = (**spinfo)->getBlockPosFromView();
    BlockPos*   pos = (BlockPos*)lua_newuserdata(L, sizeof(BlockPos));
    *pos            = rst.first;
    luaL_setmetatable(L, "blockpos_mt");
    lua_pushboolean(L, rst.second);
    return 2;
}

LUAAPI(simplayer_searchInInvWithId) {
    int count = lua_gettop(L);
    if (count < 2 || count > 3) return luaL_error(L, "1 or 2 args expected (without \"self\")");
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int id    = static_cast<int>(luaL_checkinteger(L, 2));
    int start = 0;
    if (count == 3) start = static_cast<int>(luaL_checkinteger(L, 3));
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->searchInInvWithId(id, start));
    return 1;
}

LUAAPI(simplayer_searchInInvWithName) {
    int count = lua_gettop(L);
    if (count < 2 || count > 3) return luaL_error(L, "1 or 2 args expected (without \"self\")");
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    std::string name  = luaL_checkstring(L, 2);
    int         start = 0;
    if (count == 3) start = static_cast<int>(luaL_checkinteger(L, 3));
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->searchInInvWithName(name, start));
    return 1;
}

LUAAPI(simplayer_selectSlot) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int slot = static_cast<int>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->selectSlot(slot));
    return 1;
}

LUAAPI(simplayer_select) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int id = static_cast<int>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->select(id));
    return 1;
}

LUAAPI(simplayer_getItemFromInv) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int slot = static_cast<int>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    ItemInfo** info = (ItemInfo**)lua_newuserdata(L, sizeof(ItemInfo*));
    *info           = new ItemInfo((**spinfo)->getItemFromInv(slot));
    luaL_setmetatable(L, "iteminfo_mt");
    return 1;
}

LUAAPI(simplayer_interact) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->interact());
    return 1;
}

LUAAPI(simplayer_jump) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->jump());
    return 1;
}

LUAAPI(simplayer_useItem) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int delay = static_cast<int>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    (**spinfo)->useItem(delay);
    return 0;
}

LUAAPI(simplayer_startBuild) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    (**spinfo)->startBuild();
    return 0;
}

LUAAPI(simplayer_lookAt) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    Vec3* pos = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    (**spinfo)->lookAt(*pos);
    return 0;
}

LUAAPI(simplayer_moveTo) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    Vec3* pos = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    (**spinfo)->moveTo(*pos);
    return 0;
}

LUAAPI(simplayer_navigateTo) {
    LUA_ARG_COUNT_CHECK_M(1)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    Vec3* pos = (Vec3*)luaL_checkudata(L, 2, "vec3_mt");
    luaL_argcheck(L, pos != nullptr, 2, "invalid userdata");
    lua_settop(L, 0);
    (**spinfo)->navigateTo(*pos);
    return 0;
}

LUAAPI(simplayer_isTaskFree) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushboolean(L, (**spinfo)->isTaskFree());
    return 1;
}

LUAAPI(simplayer_stopAction) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    (**spinfo)->stopAction();
    return 0;
}

LUAAPI(simplayer_getFirstEmptySlot) {
    LUA_ARG_COUNT_CHECK_M(0);
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->getFirstEmptySlot());
    return 1;
}

LUAAPI(simplayer_tryPutIntoContainer) {
    LUA_ARG_COUNT_CHECK_M(1);
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    int slot = static_cast<int>(luaL_checkinteger(L, 2));
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->tryPutIntoContainer(slot));
    return 1;
}

LUAAPI(simplayer_tryGetFromContainerWithName) {
    LUA_ARG_COUNT_CHECK_M(1);
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(
        L,
        (spinfo != nullptr) && ((*spinfo) != nullptr) && ((**spinfo) != nullptr) && ((**spinfo)->simPlayer != nullptr),
        1,
        "invalid userdata"
    );
    std::string name = luaL_checkstring(L, 2);
    lua_settop(L, 0);
    lua_pushinteger(L, (**spinfo)->tryGetFromContainerWithName(name));
    return 1;
}

LUAAPI(simplayer_meta_gc) {
    LUA_ARG_COUNT_CHECK_M(0)
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** spinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)luaL_checkudata(L, 1, "simplayer_mt");
    luaL_argcheck(L, (spinfo != nullptr) && ((*spinfo) != nullptr), 1, "invalid userdata");
    lua_settop(L, 0);
    delete *spinfo;
    return 0;
}

const luaL_Reg lua_reg_simplayer_m[37] = {
    {"getName",                     lua_api_simplayer_getName                    },
    {"getXuid",                     lua_api_simplayer_getXuid                    },
    {"getStatus",                   lua_api_simplayer_getStatus                  },
    {"getPos",                      lua_api_simplayer_getPos                     },
    {"getFeetPos",                  lua_api_simplayer_getFeetPos                 },
    {"getStandingOn",               lua_api_simplayer_getStandingOn              },
    {"getRot",                      lua_api_simplayer_getRot                     },
    {"getHealth",                   lua_api_simplayer_getHealth                  },
    {"getHunger",                   lua_api_simplayer_getHunger                  },
    {"sneaking",                    lua_api_simplayer_sneaking                   },
    {"swimming",                    lua_api_simplayer_swimming                   },
    {"attack",                      lua_api_simplayer_attack                     },
    {"chat",                        lua_api_simplayer_chat                       },
    {"destroy",                     lua_api_simplayer_destroy                    },
    {"dropSelectedItem",            lua_api_simplayer_dropSelectedItem           },
    {"dropInv",                     lua_api_simplayer_dropInv                    },
    {"runCmd",                      lua_api_simplayer_runCmd                     },
    {"getBlockPosFromView",         lua_api_simplayer_getBlockPosFromView        },
    {"searchInInvWithId",           lua_api_simplayer_searchInInvWithId          },
    {"searchInInvWithName",         lua_api_simplayer_searchInInvWithName        },
    {"selectSlot",                  lua_api_simplayer_selectSlot                 },
    {"select",                      lua_api_simplayer_select                     },
    {"getItemFromInv",              lua_api_simplayer_getItemFromInv             },
    {"interact",                    lua_api_simplayer_interact                   },
    {"jump",                        lua_api_simplayer_jump                       },
    {"useItem",                     lua_api_simplayer_useItem                    },
    {"startBuild",                  lua_api_simplayer_startBuild                 },
    {"lookAt",                      lua_api_simplayer_lookAt                     },
    {"moveTo",                      lua_api_simplayer_moveTo                     },
    {"navigateTo",                  lua_api_simplayer_navigateTo                 },
    {"isTaskFree",                  lua_api_simplayer_isTaskFree                 },
    {"stopAction",                  lua_api_simplayer_stopAction                 },
    {"getFirstEmptySlot",           lua_api_simplayer_getFirstEmptySlot          },
    {"tryPutIntoContainer",         lua_api_simplayer_tryPutIntoContainer        },
    {"tryGetFromContainerWithName", lua_api_simplayer_tryGetFromContainerWithName},
    {"__gc",                        lua_api_simplayer_meta_gc                    },
    {NULL,                          NULL                                         }
};

LUAAPI(open_simplayer) {
    luaL_newmetatable(L, "simplayer_mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lua_reg_simplayer_m, 0);
    luaL_newlib(L, lua_reg_null_c);
    return 1;
}

// userdata simplayer end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp