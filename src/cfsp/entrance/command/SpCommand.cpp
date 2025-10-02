#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandSelectorResults.h"
#include "mc/world/phys/HitResult.h"

namespace coral_fans::cfsp::command {
#define SP_ONLINE_OPERATE1_CALL(FUNC)                                                                                  \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                              \
        .output(output);

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

#define SP_ONLINE_OPERATE3_CALL(FUNC)                                                                                  \
    if (!self["times"].has_value())                                                                                    \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())                          \
            .output(output);                                                                                           \
    if (!self["interval"].has_value())                                                                                 \
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
    if (!self["interval"].has_value())                                                                                 \
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

#define SP_ONLINE_OPERATE5_CALL(FUNC)                                                                                  \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(                                                                                                     \
            player.value(),                                                                                            \
            self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                    \
            self["message"].get<ll::command::ParamKind::String>()                                                      \
        )                                                                                                              \
        .output(output);

#define SP_ONLINE_OPERATE6_CALL(FUNC)                                                                                  \
    if (!self["pos"].has_value()) {                                                                                    \
        if (!player.value()) [[unlikely]]                                                                              \
            return output.error("command.fail.lackPara"_tr());                                                         \
        const auto& hit = player.value()->traceRay(5.25f, false, true);                                                \
        if (hit)                                                                                                       \
            return manager::CFSPManager::getInstance()                                                                 \
                .sp##FUNC(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>(), hit.mPos)            \
                .output(output);                                                                                       \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                player.value()->getFeetPos()                                                                           \
            )                                                                                                          \
            .output(output);                                                                                           \
    }                                                                                                                  \
    if (!self["speed"].has_value())                                                                                    \
        return manager::CFSPManager::getInstance()                                                                     \
            .sp##FUNC(                                                                                                 \
                player.value(),                                                                                        \
                self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                \
                self["pos"]                                                                                            \
                    .get<ll::command::ParamKind::Vec3>()                                                               \
                    .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0})                                  \
            )                                                                                                          \
            .output(output);                                                                                           \
    return manager::CFSPManager::getInstance()                                                                         \
        .sp##FUNC(                                                                                                     \
            player.value(),                                                                                            \
            self["spname"].get<ll::command::ParamKind::SoftEnum>(),                                                    \
            self["pos"]                                                                                                \
                .get<ll::command::ParamKind::Vec3>()                                                                   \
                .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),                                     \
            self["speed"].get<ll::command::ParamKind::Float>()                                                         \
        )                                                                                                              \
        .output(output);

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

    // sp p create <name: string> [pos: Vec3] [dim: Dimension] [lockUniqueId: bool]
    this->command->runtimeOverload()
        .text("p")
        .text("create")
        .required("name", ll::command::ParamKind::String)
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .optional("lockUniqueId", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["pos"].has_value()) {
                if (!player.value()) return output.error("command.fail.lackPara"_tr());
                const auto& hit = player.value()->traceRay(5.25f, false, true);
                if (hit)
                    return manager::CFSPManager::getInstance()
                        .spCreate(
                            player.value(),
                            self["name"].get<ll::command::ParamKind::String>(),
                            hit.mPos,
                            player.value()->getDimensionId()
                        )
                        .output(output);
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
            if (!self["lockUniqueId"].has_value())
                return manager::CFSPManager::getInstance()
                    .spCreate(
                        player.value(),
                        self["name"].get<ll::command::ParamKind::String>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                        self["dim"].get<ll::command::ParamKind::Dimension>().id
                    )
                    .output(output);
            return manager::CFSPManager::getInstance()
                .spCreate(
                    player.value(),
                    self["name"].get<ll::command::ParamKind::String>(),
                    self["pos"]
                        .get<ll::command::ParamKind::Vec3>()
                        .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                    self["dim"].get<ll::command::ParamKind::Dimension>().id,
                    self["lockUniqueId"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });

    // sp p spawn <name: cfspOfflineSp>
    this->command->runtimeOverload()
        .text("p")
        .text("spawn")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOfflineSp")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            manager::CFSPManager::getInstance()
                .spSpawn(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                .output(output);
        });

    // sp p <despawn|stop|drop|dropinv|swap|info|invinfo> <name: cfspOnlineSp>
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate1",
        {
            {"despawn", 0},
            {"stop",    1},
            {"drop",    2},
            {"dropinv", 3},
            {"swap",    4},
            {"info",    5},
            {"invinfo", 6}
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
            case 5:
                SP_ONLINE_OPERATE1_CALL(Info)
            case 6:
                SP_ONLINE_OPERATE1_CALL(InvInfo)
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

    // sp p delete <name: cfspSplist> [force: bool]
    this->command->runtimeOverload()
        .text("p")
        .text("delete")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .optional("force", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["force"].has_value())
                return manager::CFSPManager::getInstance()
                    .spDelete(player.value(), self["spname"].get<ll::command::ParamKind::SoftEnum>())
                    .output(output);
            manager::CFSPManager::getInstance()
                .spDelete(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["force"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });

    // sp p <sneaking|swimming|flying|sprinting> <name: cfspOnlineSp> [enabled: bool]
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

    // sp p <attack|build|interact|jump> <name: cfspOnlineSp> [times: int] [interval: int]
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

    // sp p <use|destroy> <name: cfspOnlineSp> [long: int] [times: int] [interval: int]
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

    // sp p <chat|runcmd> <name: cfspOnlineSp> <message: string>
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate5",
        {
            {"chat",   0},
            {"runcmd", 1}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate5")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .required("message", ll::command::ParamKind::String)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE5_CALL(Chat)
            case 1:
                SP_ONLINE_OPERATE5_CALL(RunCmd)
            }
        });

    // sp p lookat <name: cfspOnlineSp> [pos: Vec3]
    this->command->runtimeOverload()
        .text("p")
        .text("lookat")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("pos", ll::command::ParamKind::Vec3)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["pos"].has_value()) {
                if (!player.value()) [[unlikely]]
                    return output.error("command.fail.lackPara"_tr());
                const auto& hit = player.value()->traceRay(5.25f, false, true);
                if (hit)
                    return manager ::CFSPManager ::getInstance()
                        .spLookAt(player.value(), self["spname"].get<ll ::command ::ParamKind ::SoftEnum>(), hit.mPos)
                        .output(output);
                return manager ::CFSPManager ::getInstance()
                    .spLookAt(
                        player.value(),
                        self["spname"].get<ll ::command ::ParamKind ::SoftEnum>(),
                        player.value()->getFeetPos()
                    )
                    .output(output);
            }
            return manager ::CFSPManager ::getInstance()
                .spLookAt(
                    player.value(),
                    self["spname"].get<ll ::command ::ParamKind ::SoftEnum>(),
                    self["pos"]
                        .get<ll ::command ::ParamKind ::Vec3>()
                        .getPosition(CommandVersion ::CurrentVersion(), origin, {0, 0, 0})
                )
                .output(output);
        });

    // sp p <moveto|navto> <name: cfspOnlineSp> [pos: Vec3] [speed: float]
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspOnlineSpOperate6",
        {
            {"moveto", 0},
            {"navto",  1}
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .required("operate", ll::command::ParamKind::Enum, "cfspOnlineSpOperate6")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("speed", ll::command::ParamKind::Float)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                SP_ONLINE_OPERATE6_CALL(MoveTo)
            case 1:
                SP_ONLINE_OPERATE6_CALL(NavTo)
            }
        });

    // sp p tp <name: cfspOnlineSp> <pos: Vec3> <dim: Dimension>
    this->command->runtimeOverload()
        .text("p")
        .text("tp")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            if (!self["pos"].has_value()) {
                if (!player.value()) [[unlikely]]
                    return output.error("command.fail.lackPara"_tr());
                const auto& hit = player.value()->traceRay(5.25f, false, true);
                if (hit)
                    return manager::CFSPManager::getInstance()
                        .spTp(
                            player.value(),
                            self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                            hit.mPos,
                            player.value()->getDimensionId()
                        )
                        .output(output);
                return manager::CFSPManager::getInstance()
                    .spTp(
                        player.value(),
                        self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                        player.value()->getFeetPos(),
                        player.value()->getDimensionId()
                    )
                    .output(output);
            }
            if (!self["dim"].has_value()) {
                if (!player.value())
                    return manager::CFSPManager::getInstance()
                        .spTp(
                            player.value(),
                            self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                            self["pos"]
                                .get<ll::command::ParamKind::Vec3>()
                                .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0})
                        )
                        .output(output);
                return manager::CFSPManager::getInstance()
                    .spTp(
                        player.value(),
                        self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                        player.value()->getDimensionId()
                    )
                    .output(output);
            }
            manager::CFSPManager::getInstance()
                .spTp(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["pos"]
                        .get<ll::command::ParamKind::Vec3>()
                        .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                    self["dim"].get<ll::command::ParamKind::Dimension>().id
                )
                .output(output);
        });

    // sp p select <name: cfspOnlineSp> <item: Item>
    this->command->runtimeOverload()
        .text("p")
        .text("select")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspOnlineSp")
        .required("item", ll::command::ParamKind::Item)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            return manager::CFSPManager::getInstance()
                .spSelect(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["item"].get<ll::command::ParamKind::Item>().mId
                )
                .output(output);
        });

    // sp p perm <name: cfspSplist> <permType: cfspSpPermType> <player: player> <enable: bool>
    ll::command::CommandRegistrar::getInstance().tryRegisterRuntimeEnum(
        "cfspSpPermType",
        {
            {"Spawn",          0 },
            {"Despawn",        1 },
            {"Respawn",        2 },
            {"Delete",         3 },
            {"Stop",           4 },
            {"Drop",           5 },
            {"DropInv",        6 },
            {"Swap",           7 },
            {"Sneaking",       8 },
            {"Swimming",       9 },
            {"Flying",         10},
            {"Sprinting",      11},
            {"Attack",         12},
            {"Build",          13},
            {"Interact",       14},
            {"Jump",           15},
            {"Use",            16},
            {"Destroy",        17},
            {"Chat",           18},
            {"RunCmd",         19},
            {"Select",         20},
            {"LookAt",         21},
            {"MoveTo",         22},
            {"NavTo",          23},
            {"Tp",             24},
            {"BeAddedToGroup", 25},
    }
    );
    this->command->runtimeOverload()
        .text("p")
        .text("perm")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .required("permType", ll::command::ParamKind::Enum, "cfspSpPermType")
        .required("targetPlayer", ll::command::ParamKind::Player)
        .required("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            auto targetPlayer = self["targetPlayer"].get<ll::command::ParamKind::Player>().results(origin);
            if (targetPlayer.size() > 1) return output.error("manager.fail.targetNotSingle"_tr());
            if (targetPlayer.data->data()[0]->isSimulatedPlayer()) return output.error("manager.fail.targetIsSp"_tr());
            manager::CFSPManager::getInstance()
                .spPerm(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    (simulated_player::SimPlayerPermission)(
                        1 << self["permType"].get<ll::command::ParamKind::Enum>().index
                    ),
                    self["enable"].get<ll::command::ParamKind::Bool>(),
                    targetPlayer.data->data()[0]->getUuid().asString()
                )
                .output(output);
        });

    // sp p permpublic <name: cfspSplist> <permType: cfspSpPermType> <enable: bool>
    this->command->runtimeOverload()
        .text("p")
        .text("permpublic")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .required("permType", ll::command::ParamKind::Enum, "cfspSpPermType")
        .required("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            manager::CFSPManager::getInstance()
                .spPerm(
                    player.value(),
                    self["spname"].get<ll::command::ParamKind::SoftEnum>(),
                    (simulated_player::SimPlayerPermission)(
                        1 << self["permType"].get<ll::command::ParamKind::Enum>().index
                    ),
                    self["enable"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });
}
} // namespace coral_fans::cfsp::command