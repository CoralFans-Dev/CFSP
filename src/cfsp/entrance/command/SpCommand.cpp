#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"


namespace coral_fans::cfsp::command {
void ComandManager::registerSpComand() {
    using ll::i18n_literals::operator""_tr;

    // sp list p <online|offline>
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineType",
        {
            {"online",  0},
            {"offline", 1}
    }
    );
    this->command->runtimeOverload()
        .text("list")
        .text("p")
        .optional("type", ll::command::ParamKind::Enum, "cfspOnlineType")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["type"].has_value())
                return output.success(
                    manager::CFSPManager::getInstance().listOnlineSp(player.value())
                    + manager::CFSPManager::getInstance().listOfflineSp(player.value())
                );
            switch (self["type"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                if (manager::CFSPManager::getInstance().isManager(player.value()))
                    return output.success(manager::CFSPManager::getInstance().listOnlineSp(nullptr));
                return output.success(manager::CFSPManager::getInstance().listOnlineSp(player.value()));
            case 1:
                if (manager::CFSPManager::getInstance().isManager(player.value()))
                    return output.success(manager::CFSPManager::getInstance().listOfflineSp(nullptr));
                return output.success(manager::CFSPManager::getInstance().listOfflineSp(player.value()));
            }
        });

    // sp p create <bool: lockuniqueid> <name: string> <pos: x y z> <dim: int>
    this->command->runtimeOverload()
        .text("p")
        .text("create")
        .required("name", ll::command::ParamKind::String)
        .optional("lockuniqueid", ll::command::ParamKind::Bool)
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["lockuniqueid"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                return manager::CFSPManager::getInstance()
                    .createSp(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        player.value()->getFeetPos(),
                        player.value()->getDimensionId()
                    )
                    .output(output);
            }
            if (!self["pos"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                return manager::CFSPManager::getInstance()
                    .createSp(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        player.value()->getFeetPos(),
                        player.value()->getDimensionId(),
                        self["lockuniqueid"].get<ll::command::ParamKind::Bool>()
                    )
                    .output(output);
            }
            if (!self["dim"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                return manager::CFSPManager::getInstance()
                    .createSp(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                        player.value()->getDimensionId(),
                        self["lockuniqueid"].get<ll::command::ParamKind::Bool>()
                    )
                    .output(output);
            }
            manager::CFSPManager::getInstance()
                .createSp(
                    player.value(),
                    self["name"].get<ll::command::ParamKind::String>(),
                    self["pos"]
                        .get<ll::command::ParamKind::Vec3>()
                        .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                    self["dim"].get<ll::command::ParamKind::Dimension>(),
                    self["lockuniqueid"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });

    // sp p <name: cfspOfflineSp> spawn
    this->command->runtimeOverload()
        .text("p")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOfflineSp")
        .text("spawn")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            manager::CFSPManager::getInstance()
                .spawnSp(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                .output(output);
        });

    // sp p <name: cfspOnlineSp> <despawn|respawn|stop|drop|dropinv|swap>
}
} // namespace coral_fans::cfsp::command