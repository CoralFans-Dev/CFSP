#include "ComandManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/phys/HitResult.h"
#include <vector>

namespace coral_fans::cfsp::command {
#define GROUP_ONLINE_OPERATE1_CALL(FUNC)                                                                               \
    if (self["enable"].has_value())                                                                                    \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["enable"].get<ll::command::ParamKind::Bool>()                                                         \
        );                                                                                                             \
    else                                                                                                               \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>()                                                      \
        );                                                                                                             \
    break;

#define GROUP_ONLINE_OPERATE2_CALL(FUNC)                                                                               \
    if (!self["times"].has_value())                                                                                    \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>()                                                      \
        );                                                                                                             \
    else if (!self["interval"].has_value())                                                                            \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["times"].get<ll::command::ParamKind::Int>()                                                           \
        );                                                                                                             \
    else                                                                                                               \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["times"].get<ll::command::ParamKind::Int>(),                                                          \
            self["interval"].get<ll::command::ParamKind::Int>()                                                        \
        );                                                                                                             \
    break;

#define GROUP_ONLINE_OPERATE3_CALL(FUNC)                                                                               \
    if (!self["long"].has_value())                                                                                     \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>()                                                      \
        );                                                                                                             \
    else if (!self["times"].has_value())                                                                               \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["long"].get<ll::command::ParamKind::Int>()                                                            \
        );                                                                                                             \
    else if (!self["interval"].has_value())                                                                            \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["long"].get<ll::command::ParamKind::Int>(),                                                           \
            self["times"].get<ll::command::ParamKind::Int>()                                                           \
        );                                                                                                             \
    else                                                                                                               \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["long"].get<ll::command::ParamKind::Int>(),                                                           \
            self["times"].get<ll::command::ParamKind::Int>(),                                                          \
            self["interval"].get<ll::command::ParamKind::Int>()                                                        \
        );                                                                                                             \
    break;

#define GROUP_ONLINE_OPERATE4_CALL(FUNC)                                                                               \
    if (!self["pos"].has_value()) {                                                                                    \
        if (!player.value()) [[unlikely]]                                                                              \
            return output.error("command.fail.lackPara"_tr());                                                         \
        const auto& hit = player.value()->traceRay(5.25f, false, true);                                                \
        if (hit)                                                                                                       \
            res = manager::CFSPManager::getInstance()                                                                  \
                      .group##FUNC(player.value(), self["gname"].get<ll::command::ParamKind::SoftEnum>(), hit.mPos);   \
        else                                                                                                           \
            res = manager::CFSPManager::getInstance().group##FUNC(                                                     \
                player.value(),                                                                                        \
                self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                 \
                player.value()->getFeetPos()                                                                           \
            );                                                                                                         \
    } else if (!self["speed"].has_value())                                                                             \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["pos"]                                                                                                \
                .get<ll::command::ParamKind::Vec3>()                                                                   \
                .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0})                           \
        );                                                                                                             \
    else                                                                                                               \
        res = manager::CFSPManager::getInstance().group##FUNC(                                                         \
            player.value(),                                                                                            \
            self["gname"].get<ll::command::ParamKind::SoftEnum>(),                                                     \
            self["pos"]                                                                                                \
                .get<ll::command::ParamKind::Vec3>()                                                                   \
                .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0}),                          \
            self["speed"].get<ll::command::ParamKind::Float>()                                                         \
        );                                                                                                             \
    break;

void ComandManager::registerGroupComand() {
    using ll::i18n_literals::operator""_tr;
    // sp list g
    this->command->overload().text("list").text("g").execute(
        [this](CommandOrigin const& origin, CommandOutput& output) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            output.success(manager::CFSPManager::getInstance().listGroup(player.value()));
        }
    );

    // sp g create <gname: string>
    this->command->runtimeOverload()
        .text("g")
        .text("create")
        .required("gname", ll::command::ParamKind::String)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            manager::CFSPManager::getInstance()
                .groupCreate(player.value(), self["gname"].get<ll::command::ParamKind::String>())
                .output(output);
        });

    // sp g <addsp|rmsp> <gname: cfspGroup> <spname: cfspSplist>
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate1",
        {
            {"addsp", 0},
            {"rmsp",  1}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate1")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("spname", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                return manager::CFSPManager::getInstance()
                    .groupAddSp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        self["spname"].get<ll::command::ParamKind::SoftEnum>()
                    )
                    .output(output);
            case 1:
                return manager::CFSPManager::getInstance()
                    .groupRmSp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        self["spname"].get<ll::command::ParamKind::SoftEnum>()
                    )
                    .output(output);
            }
        });

    // sp g <delete|spawn|despawn|respawn|stop|drop|dropinv|info|invinfo> <gname: cfspGroup>
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate2",
        {
            {"delete",  0},
            {"spawn",   1},
            {"despawn", 2},
            {"respawn", 3},
            {"stop",    4},
            {"info",    6},
            {"invinfo", 7}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate2")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                res.emplace_back(manager::CFSPManager::getInstance().groupDelete(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                ));
                break;
            case 1:
                res = manager::CFSPManager::getInstance().groupSpawn(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            case 2:
                res = manager::CFSPManager::getInstance().groupDespawn(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            case 3:
                res = manager::CFSPManager::getInstance().groupRespawn(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            case 4:
                res = manager::CFSPManager::getInstance().groupStop(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            case 5:
                res = manager::CFSPManager::getInstance().groupInfo(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            case 6:
                res = manager::CFSPManager::getInstance().groupInvInfo(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
                break;
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g deletesp <gname: cfspGroup> [force: bool]
    this->command->runtimeOverload()
        .text("g")
        .text("deletesp")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("force", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            if (!self["force"].has_value())
                res = manager::CFSPManager::getInstance().groupDeleteSp(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>()
                );
            else
                res = manager::CFSPManager::getInstance().groupDeleteSp(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["force"].get<ll::command::ParamKind::Bool>()
                );
            for (auto perRes : res) perRes.output(output);
        });

    // sp g <sneaking|swimming|flying|sprinting> <gname: cfspGroup> [enabled: bool]
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate3",
        {
            {"sneaking",  0},
            {"swimming",  1},
            {"flying",    2},
            {"sprinting", 3}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate3")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                GROUP_ONLINE_OPERATE1_CALL(Sneaking)
            case 1:
                GROUP_ONLINE_OPERATE1_CALL(Swimming)
            case 2:
                GROUP_ONLINE_OPERATE1_CALL(Flying)
            case 3:
                GROUP_ONLINE_OPERATE1_CALL(Sprinting)
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g <attack|build|interact|jump> <gname: cfspGroup> [times: int] [interval: int]
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate4",
        {
            {"attack",   0},
            {"build",    1},
            {"interact", 2},
            {"jump",     3},
            {"drop",     4},
            {"dropinv",  5}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate4")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("times", ll::command::ParamKind::Int)
        .optional("interval", ll::command::ParamKind::Int)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                GROUP_ONLINE_OPERATE2_CALL(Attack)
            case 1:
                GROUP_ONLINE_OPERATE2_CALL(Build)
            case 2:
                GROUP_ONLINE_OPERATE2_CALL(Interact)
            case 3:
                GROUP_ONLINE_OPERATE2_CALL(Jump)
            case 4:
                GROUP_ONLINE_OPERATE2_CALL(Drop)
            case 5:
                GROUP_ONLINE_OPERATE2_CALL(DropInv)
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g <use|destroy> <gname: cfspGroup> [long: int] [times: int] [interval: int]
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate5",
        {
            {"use",     0},
            {"destroy", 1}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate5")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("long", ll::command::ParamKind::Int)
        .optional("times", ll::command::ParamKind::Int)
        .optional("interval", ll::command::ParamKind::Int)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                GROUP_ONLINE_OPERATE3_CALL(Use)
            case 1:
                GROUP_ONLINE_OPERATE3_CALL(Destroy)
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g <chat|runcmd> <gname: cfspGroup> <message: string>
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate6",
        {
            {"chat",   0},
            {"runcmd", 1}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate6")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("message", ll::command::ParamKind::String)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                res = manager::CFSPManager::getInstance().groupChat(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["message"].get<ll::command::ParamKind::String>()
                );
                break;
            case 1:
                res = manager::CFSPManager::getInstance().groupRunCmd(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["message"].get<ll::command::ParamKind::String>()
                );
                break;
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g lookat <gname: cfspGroup> [pos: Vec3]
    this->command->runtimeOverload()
        .text("g")
        .text("lookat")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("pos", ll::command::ParamKind::Vec3)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            if (!self["pos"].has_value()) {
                if (!player.value()) [[unlikely]]
                    return output.error("command.fail.lackPara"_tr());
                const auto& hit = player.value()->traceRay(5.25f, false, true);
                if (hit)
                    res = manager ::CFSPManager ::getInstance().groupLookAt(
                        player.value(),
                        self["gname"].get<ll ::command ::ParamKind ::SoftEnum>(),
                        hit.mPos
                    );
                else
                    res = manager ::CFSPManager ::getInstance().groupLookAt(
                        player.value(),
                        self["gname"].get<ll ::command ::ParamKind ::SoftEnum>(),
                        player.value()->getFeetPos()
                    );
            } else
                res = manager ::CFSPManager ::getInstance().groupLookAt(
                    player.value(),
                    self["gname"].get<ll ::command ::ParamKind ::SoftEnum>(),
                    self["pos"]
                        .get<ll ::command ::ParamKind ::Vec3>()
                        .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0})
                );
            for (auto perRes : res) perRes.output(output);
        });

    // sp g lookat <gname: cfspGroup> <facing: cfspFacing>
    ll ::command ::CommandRegistrar ::getInstance(false).tryRegisterRuntimeEnum(
        "cfspFacing",
        {
            {"north", 0},
            {"south", 1},
            {"west",  2},
            {"east",  3},
            {"up",    4},
            {"down",  5}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .text("lookat")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("facing", ll::command::ParamKind::Enum, "cfspFacing")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            res = manager::CFSPManager::getInstance().groupLookAt(
                player.value(),
                self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                static_cast<simulated_player::SimPlayer::Direction>(
                    self["facing"].get<ll::command::ParamKind::Enum>().index
                )
            );
            for (auto perRes : res) perRes.output(output);
        });

    // sp g <moveto|navto> <gname: cfspGroup> [pos: Vec3] [speed: float]
    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupOperate7",
        {
            {"moveto", 0},
            {"navto",  1}
    }
    );
    this->command->runtimeOverload()
        .text("g")
        .required("operate", ll::command::ParamKind::Enum, "cfspGroupOperate7")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("speed", ll::command::ParamKind::Float)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            switch (self["operate"].get<ll::command::ParamKind::Enum>().index) {
            case 0:
                GROUP_ONLINE_OPERATE4_CALL(MoveTo)
            case 1:
                GROUP_ONLINE_OPERATE4_CALL(NavTo)
            }
            for (auto perRes : res) perRes.output(output);
        });

    // sp g tp <gname: cfspGroup> <pos: Vec3> <dim: Dimension>
    this->command->runtimeOverload()
        .text("g")
        .text("tp")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            std::vector<base::OperateResult> res;
            if (!self["pos"].has_value()) {
                if (!player.value()) [[unlikely]]
                    return output.error("command.fail.lackPara"_tr());
                const auto& hit = player.value()->traceRay(5.25f, false, true);
                if (hit)
                    res = manager::CFSPManager::getInstance().groupTp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        hit.mPos,
                        player.value()->getDimensionId()
                    );
                else
                    res = manager::CFSPManager::getInstance().groupTp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        player.value()->getFeetPos(),
                        player.value()->getDimensionId()
                    );
            } else if (!self["dim"].has_value()) {
                if (!player.value())
                    res = manager::CFSPManager::getInstance().groupTp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0})
                    );
                else
                    res = manager::CFSPManager::getInstance().groupTp(
                        player.value(),
                        self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0}),
                        player.value()->getDimensionId()
                    );
            } else
                res = manager::CFSPManager::getInstance().groupTp(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    self["pos"]
                        .get<ll::command::ParamKind::Vec3>()
                        .getPosition(static_cast<int>(CurrentCmdVersion::Latest), origin, {0, 0, 0}),
                    self["dim"].get<ll::command::ParamKind::Dimension>().id
                );
            for (auto perRes : res) perRes.output(output);
        });

    // sp g select <gname: cfspGroup> <item: Item>
    this->command->runtimeOverload()
        .text("g")
        .text("select")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("item", ll::command::ParamKind::Item)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            auto res = manager::CFSPManager::getInstance().groupSelect(
                player.value(),
                self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                self["item"].get<ll::command::ParamKind::Item>().mId
            );
            for (auto perRes : res) perRes.output(output);
        });

    ll::command::CommandRegistrar::getInstance(false).tryRegisterRuntimeEnum(
        "cfspGroupPermType",
        {
            {"all",       -1},
            {"AddSp",     0 },
            {"RmSp",      1 },
            {"Delete",    2 },
            {"Spawn",     3 },
            {"Despawn",   4 },
            {"Respawn",   5 },
            {"DeleteSp",  6 },
            {"Stop",      7 },
            {"Drop",      8 },
            {"DropInv",   9 },
            {"Sneaking",  10},
            {"Swimming",  11},
            {"Flying",    12},
            {"Sprinting", 13},
            {"Attack",    14},
            {"Build",     15},
            {"Interact",  16},
            {"Jump",      17},
            {"Use",       18},
            {"Destroy",   19},
            {"Chat",      20},
            {"RunCmd",    21},
            {"LookAt",    22},
            {"MoveTo",    23},
            {"NavTo",     24},
            {"Tp",        25},
            {"Select",    26},
    }
    );
    // sp g perm <gname: cfspGroup> <permType: cfspGroupPermType> <player: player> <enable: bool>
    this->command->runtimeOverload()
        .text("g")
        .text("perm")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("permType", ll::command::ParamKind::Enum, "cfspGroupPermType")
        .required("targetPlayer", ll::command::ParamKind::Player)
        .required("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            auto targetPlayer = self["targetPlayer"].get<ll::command::ParamKind::Player>().results(origin);
            if (targetPlayer.size() > 1) output.error("manager.fail.targetNotSingle"_tr());
            if (targetPlayer.data->data()[0]->isSimulatedPlayer()) return output.error("manager.fail.targetIsSp"_tr());
            auto index = self["permType"].get<ll::command::ParamKind::Enum>().index;
            uint perm;
            if (index == (uint64)-1) perm = (uint)-1;
            else perm = 1 << index;
            manager::CFSPManager::getInstance()
                .groupPerm(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    perm,
                    self["enable"].get<ll::command::ParamKind::Bool>(),
                    targetPlayer.data->data()[0]->getUuid().asString()
                )
                .output(output);
        });

    // sp g permpublic <gname: cfspGroup> <permType: cfspGroupPermType> <enable: bool>
    this->command->runtimeOverload()
        .text("g")
        .text("permpublic")
        .required("gname", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .required("permType", ll::command::ParamKind::Enum, "cfspGroupPermType")
        .required("enable", ll::command::ParamKind::Bool)
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            auto index = self["permType"].get<ll::command::ParamKind::Enum>().index;
            uint perm;
            if (index == (uint64)-1) perm = (uint)-1;
            else perm = 1 << index;
            manager::CFSPManager::getInstance()
                .groupPerm(
                    player.value(),
                    self["gname"].get<ll::command::ParamKind::SoftEnum>(),
                    perm,
                    self["enable"].get<ll::command::ParamKind::Bool>()
                )
                .output(output);
        });
}
} // namespace coral_fans::cfsp::command