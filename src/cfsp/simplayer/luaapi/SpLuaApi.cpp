#include "cfsp/CFSP.h"
#include "cfsp/base/Mod.h"
#include "cfsp/simplayer/CFSP.h"
#include "mc/network/packet/TextPacket.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "cfsp/simplayer/luaapi/BlockInfo.h"
#include "cfsp/simplayer/luaapi/BlockPos.h"
#include "cfsp/simplayer/luaapi/BlockSource.h"
#include "cfsp/simplayer/luaapi/ItemInfo.h"
#include "cfsp/simplayer/luaapi/Level.h"
#include "cfsp/simplayer/luaapi/Log.h"
#include "cfsp/simplayer/luaapi/SimPlayer.h"
#include "cfsp/simplayer/luaapi/Vec2.h"
#include "cfsp/simplayer/luaapi/Vec3.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

static const luaL_Reg lua_libs[] = {
    {"vec3",        lua_api_open_vec3       },
    {"blockpos",    lua_api_open_blockpos   },
    {"vec2",        lua_api_open_vec2       },
    {"blockinfo",   lua_api_open_blockinfo  },
    {"blocksource", lua_api_open_blocksource},
    {"level",       lua_api_open_level      },
    {"iteminfo",    lua_api_open_iteminfo   },
    {"simplayer",   lua_api_open_simplayer  },
    {NULL,          NULL                    }
};

inline void broadcastErr(std::filesystem::path const& path, std::string const& name, std::string const& msg) {
    std::string data = std::format("[CFSP-Lua] ({}@{})\n{}", path.string(), name, msg);
    TextPacket::createRawMessage(data).sendToClients();
    CFSP::getInstance().getSelf().getLogger().error(data);
}

std::pair<std::string, bool> execLuaScript(
    std::string const&                                  fileName,
    int                                                 interval,
    std::string const&                                  luaArg,
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>& spinfo,
    std::filesystem::path                               basePath
) {
    auto path = basePath / fileName;
    // new state
    std::shared_ptr<lua_State> L(luaL_newstate(), [](lua_State* state) {
        if (state) lua_close(state);
    });
    if (!L.get()) return {"Cannot open lua state", false};
    // open libs
    luaL_openlibs(L.get());
    // load log function
    lua_register(L.get(), "log", lua_api_log);
    // do preload
    int ret = luaL_dostring(L.get(), mod().getConfig().simPlayer.luaPreload.c_str());
    if (ret != LUA_OK) return {lua_tostring(L.get(), -1), false};
    lua_settop(L.get(), 0);
    // load built-in types and vars
    const luaL_Reg* libs = lua_libs;
    for (; libs->func; ++libs) {
        luaL_requiref(L.get(), libs->name, libs->func, 1);
        lua_pop(L.get(), 1);
    }
    // load BlockSource
    int* dimid = (int*)lua_newuserdata(L.get(), sizeof(int));
    *dimid     = (int)spinfo->simPlayer->getDimensionId();
    luaL_setmetatable(L.get(), "blocksource_mt");
    lua_setglobal(L.get(), "BlockSource");
    // load Level
    lua_newuserdata(L.get(), sizeof(void*));
    luaL_setmetatable(L.get(), "level_mt");
    lua_setglobal(L.get(), "Level");
    // load SimPlayer
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>** luaspinfo =
        (boost::shared_ptr<SimPlayerManager::SimPlayerInfo>**)
            lua_newuserdata(L.get(), sizeof(boost::shared_ptr<SimPlayerManager::SimPlayerInfo>*));
    *luaspinfo = new boost::shared_ptr<SimPlayerManager::SimPlayerInfo>(spinfo);
    luaL_setmetatable(L.get(), "simplayer_mt");
    lua_setglobal(L.get(), "SimPlayer");
    // set require path
    int ty = lua_getglobal(L.get(), "package");
    if (ty != LUA_TTABLE) return {"\"package\" is not a table", false};
    lua_getfield(L.get(), -1, "path");
    std::string pkgPath = lua_tostring(L.get(), -1);
    lua_pop(L.get(), 1);
    pkgPath += std::format(";{0}\\?.lua;{0}\\?\\init.lua", std::filesystem::absolute(path).parent_path().string());
    lua_pushstring(L.get(), pkgPath.c_str());
    lua_setfield(L.get(), -2, "path");
    lua_getfield(L.get(), -1, "cpath");
    std::string pkgCPath = lua_tostring(L.get(), -1);
    lua_pop(L.get(), 1);
    pkgCPath += std::format(";{0}\\?.dll;{0}\\?\\init.dll", std::filesystem::absolute(path).parent_path().string());
    lua_pushstring(L.get(), pkgCPath.c_str());
    lua_setfield(L.get(), -2, "cpath");
    lua_setglobal(L.get(), "package");
    // load and do file
    ret = luaL_dofile(L.get(), path.string().c_str());
    if (ret != LUA_OK) return {lua_tostring(L.get(), -1), false};
    lua_settop(L.get(), 0);
    // exec Init func
    int fType = lua_getglobal(L.get(), "Init");
    if (fType != LUA_TFUNCTION) return {"\"Init\" is not a function", false};
    lua_pushstring(L.get(), luaArg.c_str());
    ret = lua_pcall(L.get(), 1, 1, 0);
    if (ret != LUA_OK) return {lua_tostring(L.get(), -1), false};
    // get Init func return
    if (lua_isboolean(L.get(), -1)) {
        if (!lua_toboolean(L.get(), -1)) return {"\"Init\" function returned false. break", true}; // exit
    } else return {"function \"Init\" did not return a boolean type value", false};
    lua_settop(L.get(), 0);
    // run Tick func once
    fType = lua_getglobal(L.get(), "Tick");
    if (fType != LUA_TFUNCTION) return {"\"Tick\" is not a function", false};
    ret = lua_pcall(L.get(), 0, 1, 0);
    if (ret != LUA_OK) return {lua_tostring(L.get(), -1), false};
    if (lua_isboolean(L.get(), -1)) {
        if (!lua_toboolean(L.get(), -1)) return {"\"Tick\" function returned false. break", true}; // exit
    } else return {"function \"Tick\" did not return a boolean type value", false};
    lua_settop(L.get(), 0);
    // run Tick in scheduler
    spinfo->scriptid = spinfo->scheduler->add(interval, [L, path, spinfo](unsigned long long) {
        lua_settop(L.get(), 0);
        int fType = lua_getglobal(L.get(), "Tick");
        if (fType != LUA_TFUNCTION) {
            broadcastErr(path, spinfo->name, "\"Tick\" is not a function");
            return false;
        }
        int ret = lua_pcall(L.get(), 0, 1, 0);
        if (ret != LUA_OK) {
            broadcastErr(path, spinfo->name, lua_tostring(L.get(), -1));
            return false;
        }
        if (lua_isboolean(L.get(), -1)) return static_cast<bool>(lua_toboolean(L.get(), -1));
        else {
            broadcastErr(path, spinfo->name, "function \"Tick\" did not return a boolean type value");
            return false;
        }
    });
    // return
    return {"Success", true};
}

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp