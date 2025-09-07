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
    FuncStruct spCreate    = {true, CommandPermissionLevel::Any};
    FuncStruct spSpawn     = {true, CommandPermissionLevel::Any};
    FuncStruct spDespawn   = {true, CommandPermissionLevel::Any};
    FuncStruct spRespawn   = {true, CommandPermissionLevel::Any};
    FuncStruct spRm        = {true, CommandPermissionLevel::Any};
    FuncStruct spStop      = {true, CommandPermissionLevel::Any};
    FuncStruct spDrop      = {true, CommandPermissionLevel::Any};
    FuncStruct spDropInv   = {true, CommandPermissionLevel::Any};
    FuncStruct spSwap      = {true, CommandPermissionLevel::Any};
    FuncStruct spSneaking  = {true, CommandPermissionLevel::Any};
    FuncStruct spSwimming  = {true, CommandPermissionLevel::Any};
    FuncStruct spFlying    = {true, CommandPermissionLevel::Any};
    FuncStruct spSprinting = {true, CommandPermissionLevel::Any};
    FuncStruct spAttack    = {true, CommandPermissionLevel::Any};
    FuncStruct spBuild     = {true, CommandPermissionLevel::Any};
    FuncStruct spInteract  = {true, CommandPermissionLevel::Any};
    FuncStruct spJump      = {true, CommandPermissionLevel::Any};
    FuncStruct spUse       = {true, CommandPermissionLevel::Any};
    FuncStruct spDestroy   = {true, CommandPermissionLevel::Any};
    FuncStruct spChat      = {true, CommandPermissionLevel::Any};
    FuncStruct spRunCmd    = {true, CommandPermissionLevel::Any};
    FuncStruct spSelect    = {true, CommandPermissionLevel::Any};
    FuncStruct spLookAt    = {true, CommandPermissionLevel::Any};
    FuncStruct spMoveTo    = {true, CommandPermissionLevel::Any};
    FuncStruct spNavTo     = {true, CommandPermissionLevel::Any};
    FuncStruct spTp        = {true, CommandPermissionLevel::Any};
    FuncStruct groupCreate = {true, CommandPermissionLevel::Any};
};
} // namespace coral_fans::cfsp::config