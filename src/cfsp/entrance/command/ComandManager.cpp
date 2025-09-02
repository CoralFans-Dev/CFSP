#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"
#include <optional>
#include <string>


namespace coral_fans::cfsp::command {
ComandManager& ComandManager::getInstance() {
    static ComandManager instance;
    return instance;
}

std::optional<Player*> ComandManager::tryGetPlayer(CommandOrigin const& origin) {
    auto oriType = origin.getOriginType();
    if (oriType == CommandOriginType::DedicatedServer) return nullptr;
    if (oriType != CommandOriginType::Player) return std::nullopt;
    auto* entity = origin.getEntity();
    if (entity == nullptr || !entity->isType(ActorType::Player)) return std::nullopt;
    return static_cast<Player*>(entity);
}

void ComandManager::registerCommand(CommandPermissionLevel permission) {
    using ll::i18n_literals::operator""_tr;

    // reg cmd
    this->command = &ll::command::CommandRegistrar::getInstance()
                         .getOrCreateCommand("sp", "command.sp.description"_tr(), permission);

    // sp version
    this->command->overload().text("version").execute([](CommandOrigin const&, CommandOutput& output) {
#ifdef VERSION
        output.success(VERSION);
#endif
#ifdef COMMITID
        output.success("Commit ID: {}", COMMITID);
#endif
    });

    // sp c <autojoin|autorespawn|autodespawn> <isopen: bool>
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspSettingType",
        {
            {"autojoin",    0},
            {"autorespawn", 1},
            {"autodespawn", 2}
    }
    );
    this->command->runtimeOverload()
        .text("c")
        .required("type", ll::command::ParamKind::Enum, "cfspSettingType")
        .required("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value() || !manager::CFSPManager::getInstance().isManager(player.value()))
                return output.error("command.fail.illegalOrigin"_tr());
            switch (self["type"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                manager::CFSPManager::getInstance().setAutoJoin(self["enable"].get<ll::command::ParamKind::Bool>());
                break;
            case 1:
                manager::CFSPManager::getInstance().setAutoRespawn(self["enable"].get<ll::command::ParamKind::Bool>());
                break;
            case 2:
                manager::CFSPManager::getInstance().setAutoDespawn(self["enable"].get<ll::command::ParamKind::Bool>());
                break;
            }
            output.success("manager.success.set"_tr());
        });

    this->registerGuiCommand();
    this->registerSpComand();
}
} // namespace coral_fans::cfsp::command