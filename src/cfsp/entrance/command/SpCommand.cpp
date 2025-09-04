#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::command {
void ComandManager::registerSpComand() {
    using ll::i18n_literals::operator""_tr;

    // sp list p [online|offline]
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
            if (!manager::CFSPManager::getInstance().isAllowed(player.value()))
                return output.error("command.error.permissionDenied"_tr());
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

    // sp p create <name: string> [pos: x y z] [dim: Dimension]
    this->command->runtimeOverload()
        .text("p")
        .text("create")
        .required("name", ll::command::ParamKind::String)
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["pos"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                return manager::CFSPManager::getInstance()
                    .spCreate(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        player.value()->getFeetPos(),
                        player.value()->getDimensionId()
                    )
                    .output(output);
            }
            if (!self["dim"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                return manager::CFSPManager::getInstance()
                    .spCreate(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                        player.value()->getDimensionId()
                    )
                    .output(output);
            }
            manager::CFSPManager::getInstance()
                .spCreate(
                    player.value(),
                    self["name"].get<ll::command::ParamKind::String>(),
                    self["pos"]
                        .get<ll::command::ParamKind::Vec3>()
                        .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                    self["dim"].get<ll::command::ParamKind::Dimension>()
                )
                .output(output);
        });

    // sp p spawn <name: cfspOfflineSp> [bool: lockuniqueid]
    this->command->runtimeOverload()
        .text("p")
        .text("spawn")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOfflineSp")
        .optional("lockuniqueid", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["lockuniqueid"].has_value())
                return manager::CFSPManager::getInstance()
                    .spSpawn(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                    .output(output);
            manager::CFSPManager::getInstance()
                .spSpawn(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["lockuniqueid"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });

    // sp p <despawn|stop|drop|dropinv|swap> <name: cfspOnlineSp>
#define SP_ONLINE_OPERATE1_CALL(FUNC)                                                                                  \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                              \
        .output(output);

    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate1",
        {
            {"despawn", 0},
            {"stop",    1},
            {"drop",    2},
            {"dropinv", 3},
            {"swap",    4}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate1")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE1_CALL(Despawn)
            case 1:
                SP_ONLINE_OPERATE1_CALL(Stop)
            case 2:
                SP_ONLINE_OPERATE1_CALL(Drop)
            case 3:
                SP_ONLINE_OPERATE1_CALL(DropInv)
            case 4:
                if (!player.has_value()) return output.error("command.fail.onlyplayer"_tr());
                SP_ONLINE_OPERATE1_CALL(Swap)
            }
        });

    // sp p respawn <name: cfspDeadSp>
    this->command->runtimeOverload()
        .text("p")
        .text("respawn")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspDeadSp")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            manager::CFSPManager::getInstance()
                .spRespawn(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                .output(output);
        });

    // sp p rm <name: cfspSplist> [bool: force]
    this->command->runtimeOverload()
        .text("p")
        .text("rm")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .optional("force", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["force"].has_value())
                return manager::CFSPManager::getInstance()
                    .spRm(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                    .output(output);
            manager::CFSPManager::getInstance()
                .spRm(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    false,
                    self["force"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });

    // sp p <sneaking|swimming|flying|sprinting> <name: cfspOnlineSp> [bool: enabled]
#define SP_ONLINE_OPERATE2_CALL(FUNC)                                                                                  \
    if (self["enable"].has_value())                                                                                    \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                self["enable"].get<ll::command::ParamKind::Bool>()                                                     \
            )                                                                                                          \
            .output(output);                                                                                           \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                              \
        .output(output);

    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate2",
        {
            {"sneaking",  0},
            {"swimming",  1},
            {"flying",    2},
            {"sprinting", 3}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate2")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());

            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE2_CALL(Sneaking)
            case 1:
                SP_ONLINE_OPERATE2_CALL(Swimming)
            case 2:
                SP_ONLINE_OPERATE2_CALL(Flying)
            case 3:
                SP_ONLINE_OPERATE2_CALL(Sprinting)
            }
        });

    // sp p <attack|build|interact|jump> <name: cfspOnlineSp> [int: times] [int: interval]
#define SP_ONLINE_OPERATE3_CALL(FUNC)                                                                                  \
    if (!self["times"].has_value())                                                                                    \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                          \
            .output(output);                                                                                           \
    else if (!self["interval"].has_value())                                                                            \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                self["times"].get<ll::command::ParamKind::Int>()                                                       \
            )                                                                                                          \
            .output(output);                                                                                           \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(                                                                                                     \
            player.value(),                                                                                            \
            self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                    \
            self["times"].get<ll::command::ParamKind::Int>(),                                                          \
            self["interval"].get<ll::command::ParamKind::Int>()                                                        \
        )                                                                                                              \
        .output(output);

    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate3",
        {
            {"attack",   0},
            {"build",    1},
            {"interact", 2},
            {"jump",     3}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate3")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("times", ll::command::ParamKind::Int)
        .optional("interval", ll::command::ParamKind::Int)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE3_CALL(Attack)
            case 1:
                SP_ONLINE_OPERATE3_CALL(Build)
            case 2:
                SP_ONLINE_OPERATE3_CALL(Interact)
            case 3:
                SP_ONLINE_OPERATE3_CALL(Jump)
            }
        });

    // sp p <use|destroy> <name: cfspOnlineSp> [int: long] [int: times] [int: interval]
#define SP_ONLINE_OPERATE4_CALL(FUNC)                                                                                  \
    if (!self["long"].has_value())                                                                                     \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                          \
            .output(output);                                                                                           \
    if (!self["times"].has_value())                                                                                    \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                self["long"].get<ll::command::ParamKind::Int>()                                                        \
            )                                                                                                          \
            .output(output);                                                                                           \
    else if (!self["interval"].has_value())                                                                            \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                self["long"].get<ll::command::ParamKind::Int>(),                                                       \
                self["times"].get<ll::command::ParamKind::Int>()                                                       \
            )                                                                                                          \
            .output(output);                                                                                           \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(                                                                                                     \
            player.value(),                                                                                            \
            self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                    \
            self["long"].get<ll::command::ParamKind::Int>(),                                                           \
            self["times"].get<ll::command::ParamKind::Int>(),                                                          \
            self["interval"].get<ll::command::ParamKind::Int>()                                                        \
        )                                                                                                              \
        .output(output);

    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate4",
        {
            {"use",     0},
            {"destroy", 1}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate4")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("long", ll::command::ParamKind::Int)
        .optional("times", ll::command::ParamKind::Int)
        .optional("interval", ll::command::ParamKind::Int)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE4_CALL(Use)
            case 1:
                SP_ONLINE_OPERATE4_CALL(Destroy)
            }
        });
}
} // namespace coral_fans::cfsp::command