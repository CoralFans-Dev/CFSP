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
    std::string                     namePrefix      = "SIM-";
    std::string                     namePostfix     = "";
    unsigned long long              maxOnline       = 16;
    unsigned long long              maxOwn          = 4;
    unsigned long long              maxGroup        = 8;
    unsigned long long              maxSpawnCount   = 128;
    CommandPermissionLevel          adminPermission = CommandPermissionLevel::GameDirectors;
    ListType                        listType        = ListType::disabled;
    std::unordered_set<std::string> list;
    std::string                     luaPreload = "";
};

struct Config {
    int         version    = 1;
    std::string locateName = "zh_CN";

    SimPlayerStruct simPlayer;

    CommandStruct command;
};

} // namespace coral_fans::cfsp::config