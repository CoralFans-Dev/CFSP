#pragma once

#include "ll/api/command/CommandHandle.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include <optional>

namespace coral_fans::cfsp::command {
class ComandManager {
private:
    ll::command::CommandHandle* command;

public:
    static ComandManager& getInstance();

private:
    // 从指令源中获取玩家，如果指令来源是控制台则返回null
    std::optional<Player*> tryGetPlayer(CommandOrigin const& origin);
    void                   registerGuiCommand();
    void                   registerSpComand();
    void                   registerGroupComand();

public:
    void registerCommand(CommandPermissionLevel permission);
};
} // namespace coral_fans::cfsp::command