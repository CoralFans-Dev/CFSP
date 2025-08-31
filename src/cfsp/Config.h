#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"
#include <string>
#include <unordered_set>

namespace coral_fans::cfsp::config {


enum class ListType : int { disabled, blacklist, whitelist };

struct Config {
    int                             version             = 3;
    bool                            enabled             = true;
    CommandPermissionLevel          permission          = CommandPermissionLevel::Any;
    std::string                     namePrefix          = "SIM-";
    std::string                     namePostfix         = "";
    bool                            autoRespawn         = true;
    bool                            autoJoin            = true;
    bool                            autoDespawn         = true;
    unsigned long long              maxOnline           = 16;
    unsigned long long              maxOwn              = 10;
    unsigned long long              maxOnlinePerPlayer  = 3;
    unsigned long long              maxGroup            = 5;
    unsigned long long              autoDespawnCount    = 3;
    unsigned long long              autoDespawninterval = 600;
    CommandPermissionLevel          adminPermission     = CommandPermissionLevel::GameDirectors;
    ListType                        listType            = ListType::disabled;
    std::unordered_set<std::string> list;
    std::unordered_set<std::string> superManagerList;
    std::string                     luaPreload = "";
};

} // namespace coral_fans::cfsp::config