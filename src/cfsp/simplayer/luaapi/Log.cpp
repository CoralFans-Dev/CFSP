#include "boost/algorithm/string/join.hpp"
#include "cfsp/CFSP.h"
#include "cfsp/base/Macros.h"
// #include "cfsp/simplayer/CFSP.h"
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

LUAAPI(log) {
    int                    count = lua_gettop(L);
    std::list<std::string> lst;
    for (int i = 1; i <= count; ++i) lst.emplace_back(luaL_checkstring(L, i));
    CFSP::getInstance().getSelf().getLogger().info("[CFSP-Lua] {}", boost::algorithm::join(lst, "\t"));
    return 0;
}

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp