#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"

namespace coral_fans::cfsp::config {
struct FuncStruct {
    bool                   enabled;
    CommandPermissionLevel permission;
};

struct PermissionConfig {
    int        version         = 1;
    FuncStruct gui             = {true, CommandPermissionLevel::Any};
    FuncStruct spCreate        = {true, CommandPermissionLevel::Any};
    FuncStruct spSpawn         = {true, CommandPermissionLevel::Any};
    FuncStruct spDespawn       = {true, CommandPermissionLevel::Any};
    FuncStruct spRespawn       = {true, CommandPermissionLevel::Any};
    FuncStruct spDelete        = {true, CommandPermissionLevel::Any};
    FuncStruct spStop          = {true, CommandPermissionLevel::Any};
    FuncStruct spDrop          = {true, CommandPermissionLevel::Any};
    FuncStruct spDropInv       = {true, CommandPermissionLevel::Any};
    FuncStruct spSwap          = {true, CommandPermissionLevel::Any};
    FuncStruct spSneaking      = {true, CommandPermissionLevel::Any};
    FuncStruct spSwimming      = {true, CommandPermissionLevel::Any};
    FuncStruct spFlying        = {true, CommandPermissionLevel::Any};
    FuncStruct spSprinting     = {true, CommandPermissionLevel::Any};
    FuncStruct spAttack        = {true, CommandPermissionLevel::Any};
    FuncStruct spBuild         = {true, CommandPermissionLevel::Any};
    FuncStruct spInteract      = {true, CommandPermissionLevel::Any};
    FuncStruct spJump          = {true, CommandPermissionLevel::Any};
    FuncStruct spUse           = {true, CommandPermissionLevel::Any};
    FuncStruct spDestroy       = {true, CommandPermissionLevel::Any};
    FuncStruct spChat          = {true, CommandPermissionLevel::Any};
    FuncStruct spRunCmd        = {true, CommandPermissionLevel::Any};
    FuncStruct spSelect        = {true, CommandPermissionLevel::Any};
    FuncStruct spLookAt        = {true, CommandPermissionLevel::Any};
    FuncStruct spMoveTo        = {true, CommandPermissionLevel::Any};
    FuncStruct spNavTo         = {true, CommandPermissionLevel::Any};
    FuncStruct spTp            = {true, CommandPermissionLevel::Any};
    FuncStruct spPerm          = {true, CommandPermissionLevel::Any};
    FuncStruct spPublicPerm    = {true, CommandPermissionLevel::Any};
    FuncStruct groupCreate     = {true, CommandPermissionLevel::Any};
    FuncStruct groupDelete     = {true, CommandPermissionLevel::Any};
    FuncStruct groupAddSp      = {true, CommandPermissionLevel::Any};
    FuncStruct groupRmSp       = {true, CommandPermissionLevel::Any};
    FuncStruct groupSpawn      = {true, CommandPermissionLevel::Any};
    FuncStruct groupDespawn    = {true, CommandPermissionLevel::Any};
    FuncStruct groupRespawn    = {true, CommandPermissionLevel::Any};
    FuncStruct groupDeleteSp   = {true, CommandPermissionLevel::Any};
    FuncStruct groupStop       = {true, CommandPermissionLevel::Any};
    FuncStruct groupDrop       = {true, CommandPermissionLevel::Any};
    FuncStruct groupDropInv    = {true, CommandPermissionLevel::Any};
    FuncStruct groupSneaking   = {true, CommandPermissionLevel::Any};
    FuncStruct groupSwimming   = {true, CommandPermissionLevel::Any};
    FuncStruct groupFlying     = {true, CommandPermissionLevel::Any};
    FuncStruct groupSprinting  = {true, CommandPermissionLevel::Any};
    FuncStruct groupAttack     = {true, CommandPermissionLevel::Any};
    FuncStruct groupBuild      = {true, CommandPermissionLevel::Any};
    FuncStruct groupInteract   = {true, CommandPermissionLevel::Any};
    FuncStruct groupJump       = {true, CommandPermissionLevel::Any};
    FuncStruct groupUse        = {true, CommandPermissionLevel::Any};
    FuncStruct groupDestroy    = {true, CommandPermissionLevel::Any};
    FuncStruct groupChat       = {true, CommandPermissionLevel::Any};
    FuncStruct groupRunCmd     = {true, CommandPermissionLevel::Any};
    FuncStruct groupLookAt     = {true, CommandPermissionLevel::Any};
    FuncStruct groupMoveTo     = {true, CommandPermissionLevel::Any};
    FuncStruct groupNavTo      = {true, CommandPermissionLevel::Any};
    FuncStruct groupTp         = {true, CommandPermissionLevel::Any};
    FuncStruct groupSelect     = {true, CommandPermissionLevel::Any};
    FuncStruct groupPerm       = {true, CommandPermissionLevel::Any};
    FuncStruct groupPublicPerm = {true, CommandPermissionLevel::Any};
};
} // namespace coral_fans::cfsp::config