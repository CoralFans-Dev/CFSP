#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"

namespace coral_fans::cfsp::config {
struct FuncStruct {
    bool                   enabled;
    CommandPermissionLevel permission;
};

struct BaseFunc {
    bool       enabled;
    FuncStruct create = {true, CommandPermissionLevel::Any};
};

struct PermissionConfig {
    int        version = 1;
    FuncStruct gui     = {true, CommandPermissionLevel::Any};
    BaseFunc   base;
};
} // namespace coral_fans::cfsp::config