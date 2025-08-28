#include "ComandManager.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"
#include <optional>

namespace coral_fans::cfsp::command {
ComandManager& ComandManager::getInstance() {
    static ComandManager instance;
    return instance;
}

std::optional<Player*> ComandManager::tryGetPlayer(CommandOrigin const& origin) {
    auto* entity = origin.getEntity();
    if (entity == nullptr || !entity->isType(ActorType::Player)) return std::nullopt;
    return static_cast<Player*>(entity);
}

void ComandManager::registerCommand(CommandPermissionLevel permission) {
    using ll::i18n_literals::operator""_tr;

    // reg cmd
    auto& spCommand = ll::command::CommandRegistrar::getInstance()
                          .getOrCreateCommand("sp", "command.sp.description"_tr(), permission);

    // sp version
    spCommand.overload().text("version").execute([](CommandOrigin const&, CommandOutput& output) {
#ifdef VERSION
        output.success(VERSION);
#endif
#ifdef COMMITID
        output.success("Commit ID: {}", COMMITID);
#endif
    });

    this->registerGuiCommand(spCommand);
}
} // namespace coral_fans::cfsp::command