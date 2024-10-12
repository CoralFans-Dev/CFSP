#pragma once

#include "cfsp/base/Macros.h"

extern "C" {
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

int lua_api_log(lua_State*);

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp