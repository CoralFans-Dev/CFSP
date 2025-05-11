#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"
#include <string>
#include <unordered_set>

namespace coral_fans::cfsp::config {

struct CommandConfigStruct {
    bool                   enabled;
    CommandPermissionLevel permission;
};

struct CommandStruct {
    CommandConfigStruct sp = {true, CommandPermissionLevel::Any};
};

enum class ListType : int { disabled, blacklist, whitelist };

struct SimPlayerStruct {
    std::string                     namePrefix          = "SIM-";
    std::string                     namePostfix         = "";
    bool                            allowTp             = false;
    unsigned long long              maxOnline           = 16;
    unsigned long long              maxOwn              = 10;
    unsigned long long              maxOnlinePerPlayer  = 3;
    unsigned long long              maxGroup            = 5;
    unsigned long long              maxSpawnCount       = 128;
    unsigned long long              autoDespawnCount    = 3;
    unsigned long long              autoDespawninterval = 600;
    CommandPermissionLevel          adminPermission     = CommandPermissionLevel::GameDirectors;
    ListType                        listType            = ListType::disabled;
    std::unordered_set<std::string> list;
    std::unordered_set<std::string> superManagerList;
    std::string                     luaPreload = "";
};

struct Config {
    int         version    = 2;
    std::string locateName = "zh_CN";

    SimPlayerStruct simPlayer;

    CommandStruct command;
};

} // namespace coral_fans::cfsp::config