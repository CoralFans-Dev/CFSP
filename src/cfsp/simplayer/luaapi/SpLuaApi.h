#pragma once

#include "cfsp/simplayer/CFSP.h"
#include <boost/shared_ptr.hpp>
#include <filesystem>
#include <string>
#include <utility>

namespace coral_fans::cfsp {

namespace sputils::lua_api {

std::pair<std::string, bool> execLuaScript(
    std::string const&,
    int,
    std::string const&,
    boost::shared_ptr<SimPlayerManager::SimPlayerInfo>,
    std::filesystem::path
);

}

} // namespace coral_fans::cfsp