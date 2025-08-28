#pragma once

#include "ll/api/command/CommandHandle.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include <optional>

namespace coral_fans::cfsp::command {
class ComandManager {
public:
    static ComandManager& getInstance();

private:
    std::optional<Player*> tryGetPlayer(CommandOrigin const& origin);
    void                   registerGuiCommand(ll::command::CommandHandle& command);

public:
    void registerCommand(CommandPermissionLevel permission);
};
} // namespace coral_fans::cfsp::command