#pragma once

#include "mc/server/commands/CommandPermissionLevel.h"

namespace coral_fans::cfsp::config {
struct CommandStruct {
    bool                   enabled;
    CommandPermissionLevel permission;
};

struct CommandPermission {
    int           version = 1;
    CommandStruct sp      = {true, CommandPermissionLevel::Any};
};
} // namespace coral_fans::cfsp::config