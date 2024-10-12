#pragma once

#include <string>
#include <utility>

namespace coral_fans::cfsp {

namespace sputils::lua_api {

std::pair<std::string, bool>
execLuaScript(std::string const& fileName, int interval, std::string const&, SimPlayerManager::SimPlayerInfo& spinfo);

}

} // namespace coral_fans::cfsp