#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"

namespace coral_fans::cfsp::config {
struct FuncStruct {
    bool                   enabled;
    CommandPermissionLevel permission;
};

struct PermissionConfig {
    int        version     = 1;
    FuncStruct gui         = {true, CommandPermissionLevel::Any};
    FuncStruct createSp    = {true, CommandPermissionLevel::Any};
    FuncStruct spawnSp     = {true, CommandPermissionLevel::Any};
    FuncStruct despawnSp   = {true, CommandPermissionLevel::Any};
    FuncStruct respawnSp   = {true, CommandPermissionLevel::Any};
    FuncStruct rmSp        = {true, CommandPermissionLevel::Any};
    FuncStruct stopSp      = {true, CommandPermissionLevel::Any};
    FuncStruct dropSp      = {true, CommandPermissionLevel::Any};
    FuncStruct dropInvSp   = {true, CommandPermissionLevel::Any};
    FuncStruct swapSp      = {true, CommandPermissionLevel::Any};
    FuncStruct createGroup = {true, CommandPermissionLevel::Any};
};
} // namespace coral_fans::cfsp::config