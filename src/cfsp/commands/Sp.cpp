#include "cfsp/Config.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/Mod.h"
#include "cfsp/gui/GuiForm.h"
#include "cfsp/simplayer/CFSP.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandVersion.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/phys/HitResultType.h"
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <utility>


namespace {

inline void regSubCmd(
    ll::command::CommandHandle&                                                              cmd,
    std::string const&                                                                       tag,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                   rArgs,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                   oArgs,
    std::function<std::pair<std::string, bool>(Player*, ll::command::RuntimeCommand const&)> spFn,
    std::function<std::pair<std::string, bool>(Player*, ll::command::RuntimeCommand const&)> gFn
) {
    using ll::i18n_literals::operator""_tr;
    auto ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(
        cmd.runtimeOverload().text("p").required("name", ll::command::ParamKind::SoftEnum, "spname").text(tag)
    ));
    auto ro2 = std::make_unique<ll::command::RuntimeOverload>(
        std::move(cmd.runtimeOverload().text("g").required("name", ll::command::ParamKind::SoftEnum, "gname").text(tag))
    );
    for (const auto& i : rArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->required(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->required(i.first, i.second)));
    }
    for (const auto& i : oArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->optional(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->optional(i.first, i.second)));
    }
    ro1->execute([spFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        COMMAND_CHECK_PLAYER
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = spFn(player, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
    ro2->execute([gFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        COMMAND_CHECK_PLAYER
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = gFn(player, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
}

inline void regSubCmd(
    ll::command::CommandHandle&                                                                           cmd,
    std::string const&                                                                                    tag,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                                rArgs,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                                oArgs,
    std::function<std::pair<std::string, bool>(CommandOrigin const&, ll::command::RuntimeCommand const&)> spFn,
    std::function<std::pair<std::string, bool>(CommandOrigin const&, ll::command::RuntimeCommand const&)> gFn
) {
    using ll::i18n_literals::operator""_tr;
    auto ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(
        cmd.runtimeOverload().text("p").required("name", ll::command::ParamKind::SoftEnum, "spname").text(tag)
    ));
    auto ro2 = std::make_unique<ll::command::RuntimeOverload>(
        std::move(cmd.runtimeOverload().text("g").required("name", ll::command::ParamKind::SoftEnum, "gname").text(tag))
    );
    for (const auto& i : rArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->required(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->required(i.first, i.second)));
    }
    for (const auto& i : oArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->optional(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->optional(i.first, i.second)));
    }
    ro1->execute([spFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        COMMAND_CHECK_PLAYER
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = spFn(origin, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
    ro2->execute([gFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        COMMAND_CHECK_PLAYER
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = gFn(origin, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
}

inline void regSubCmd2(
    ll::command::CommandHandle&                                                              cmd,
    std::string const&                                                                       tag,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                   rArgs,
    std::span<std::pair<std::string, ll::command::ParamKind::Kind>> const&                   oArgs,
    std::function<std::pair<std::string, bool>(Player*, ll::command::RuntimeCommand const&)> spFn,
    std::function<std::pair<std::string, bool>(Player*, ll::command::RuntimeCommand const&)> gFn
) {
    using ll::i18n_literals::operator""_tr;
    auto ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(
        cmd.runtimeOverload().text("p").required("name", ll::command::ParamKind::SoftEnum, "spname").text(tag)
    ));
    auto ro2 = std::make_unique<ll::command::RuntimeOverload>(
        std::move(cmd.runtimeOverload().text("g").required("name", ll::command::ParamKind::SoftEnum, "gname").text(tag))
    );
    for (const auto& i : rArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->required(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->required(i.first, i.second)));
    }
    for (const auto& i : oArgs) {
        ro1 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro1->optional(i.first, i.second)));
        ro2 = std::make_unique<ll::command::RuntimeOverload>(std::move(ro2->optional(i.first, i.second)));
    }
    ro1->execute([spFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        auto*   entity = origin.getEntity();
        Player* player;
        if (entity == nullptr) player = nullptr;
        else if (!entity->isType(ActorType ::Player)) {
            output.error("command.sp.error.onlyplayer"_tr());
            return;
        } else player = static_cast<Player*>(entity);
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = spFn(player, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
    ro2->execute([gFn](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
        auto*   entity = origin.getEntity();
        Player* player;
        if (entity == nullptr) player = nullptr;
        else if (!entity->isType(ActorType ::Player)) {
            output.error("command.sp.error.onlyplayer"_tr());
            return;
        } else player = static_cast<Player*>(entity);
        COMMAND_SIMPLAYER_CHECKPERMLIST
        auto rst = gFn(player, self);
        if (rst.second) output.success(rst.first);
        else output.error(rst.first);
    });
}
} // namespace

namespace coral_fans::cfsp::commands {

void registerSpCommand(CommandPermissionLevel permission) {
    using ll::i18n_literals::operator""_tr;

    // reg cmd
    auto& spCommand = ll::command::CommandRegistrar::getInstance()
                          .getOrCreateCommand("sp", "command.sp.description"_tr(), permission);

    // reg softenum
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("spname", {});
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("gname", {});

    // sp version
    spCommand.overload().text("version").execute([](CommandOrigin const&, CommandOutput& output) {
#ifdef VERSION
        output.success(VERSION);
#endif
#ifdef COMMITID
        output.success("Commit ID: {}", COMMITID);
#endif
        output.success("SimPlayerManager Version: {}\n - SimPlayerInfo Version: {}", MANAGER_VERSION, INFO_VERSION);
    });

    // sp
    spCommand.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        COMMAND_CHECK_PLAYER
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName))
            coral_fans::cfsp::gui::sendManagerMainMenu(player);
        else {
            auto grouplist = SimPlayerManager::getInstance().fetchGroupList(player);
            if (grouplist.size()) coral_fans::cfsp::gui::sendMainMenu(player);
            else coral_fans::cfsp::gui::sendSplist(player);
        }
    });

    // sp p <name: string>
    spCommand.runtimeOverload()
        .text("p")
        .optional("name", ll::command::ParamKind::SoftEnum, "spname")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            if (self["name"].has_value()) {
                auto tem =
                    SimPlayerManager::getInstance().fetchSimPlayer(self["name"].get<ll::command::ParamKind::SoftEnum>()
                    );
                if (tem.has_value()) {
                    std::string UUID = player->getUuid().asString();
                    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
                        || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)
                        || tem.value()->ownerUuid == UUID)
                        coral_fans::cfsp::gui::sendSpOperatorList(player, tem.value());
                    else return output.error("sommand.sp.error.permissiondenied"_tr());
                } else return output.error("sommand.sp.error.spnofind"_tr());
            } else coral_fans::cfsp::gui::sendSplist(player);
            return output.success("command.sp.success"_tr());
        });

    // sp g <name: string>
    spCommand.runtimeOverload()
        .text("g")
        .optional("name", ll::command::ParamKind::SoftEnum, "gname")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            if (self["name"].has_value()) {
                auto tem =
                    SimPlayerManager::getInstance().fetchGroup(self["name"].get<ll::command::ParamKind::SoftEnum>());
                if (tem.has_value()) {
                    std::string UUID = player->getUuid().asString();
                    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
                        || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)
                        || tem.value()->owner == UUID || tem.value()->admin.contains(UUID))
                        coral_fans::cfsp::gui::sendGroupOperatorList(player, tem.value());
                    else return output.error("sommand.sp.error.permissiondenied"_tr());
                } else return output.error("sommand.sp.error.groupnofind"_tr());
            } else coral_fans::cfsp::gui::sendGroupList(player);
            return output.success("command.sp.success"_tr());
        });

    // sp c autorespawn <isopen: bool>
    spCommand.runtimeOverload()
        .text("c")
        .text("autorespawn")
        .required("isopen", ll::command::ParamKind::Bool)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto& mod = coral_fans::cfsp::mod();
            if (origin.getPermissionsLevel() >= mod.getConfig().simPlayer.adminPermission) {
                SimPlayerManager::getInstance().setAutoRespawn(self["isopen"].get<ll::command::ParamKind::Bool>());
                return output.success("command.sp.success"_tr());
            } else {
                auto* entity = origin.getEntity();
                if (entity != nullptr || entity->isType(ActorType::Player)) {
                    auto* player = static_cast<Player*>(entity);
                    if (mod.getConfig().simPlayer.superManagerList.contains(*player->mName)) {
                        SimPlayerManager::getInstance().setAutoRespawn(self["isopen"].get<ll::command::ParamKind::Bool>(
                        ));
                        return output.success("command.sp.success"_tr());
                    }
                }
            }
            output.error("command.sp.error.permissiondenied"_tr());
        });

    // sp c autojoin <isopen: bool>
    spCommand.runtimeOverload()
        .text("c")
        .text("autojoin")
        .required("isopen", ll::command::ParamKind::Bool)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto& mod = coral_fans::cfsp::mod();
            if (origin.getPermissionsLevel() >= mod.getConfig().simPlayer.adminPermission) {
                SimPlayerManager::getInstance().setAutoJoin(self["isopen"].get<ll::command::ParamKind::Bool>());
                return output.success("command.sp.success"_tr());
            } else {
                auto* entity = origin.getEntity();
                if (entity != nullptr || entity->isType(ActorType::Player)) {
                    auto* player = static_cast<Player*>(entity);
                    if (mod.getConfig().simPlayer.superManagerList.contains(*player->mName)) {
                        SimPlayerManager::getInstance().setAutoJoin(self["isopen"].get<ll::command::ParamKind::Bool>());
                        return output.success("command.sp.success"_tr());
                    }
                }
            }
            output.error("command.sp.error.permissiondenied"_tr());
        });

    // sp c autodespawn <isopen: bool>
    spCommand.runtimeOverload()
        .text("c")
        .text("autodespawn")
        .required("isopen", ll::command::ParamKind::Bool)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto& mod = coral_fans::cfsp::mod();
            if (origin.getPermissionsLevel() >= mod.getConfig().simPlayer.adminPermission) {
                SimPlayerManager::getInstance().setAutoDespawn(self["isopen"].get<ll::command::ParamKind::Bool>());
                return output.success("command.sp.success"_tr());
            } else {
                auto* entity = origin.getEntity();
                if (entity != nullptr || entity->isType(ActorType::Player)) {
                    auto* player = static_cast<Player*>(entity);
                    if (mod.getConfig().simPlayer.superManagerList.contains(*player->mName)) {
                        SimPlayerManager::getInstance().setAutoDespawn(self["isopen"].get<ll::command::ParamKind::Bool>(
                        ));
                        return output.success("command.sp.success"_tr());
                    }
                }
            }
            output.error("command.sp.error.permissiondenied"_tr());
        });

    // sp list g
    spCommand.runtimeOverload().text("list").text("g").execute(
        [](CommandOrigin const&, CommandOutput& output, ll::command::RuntimeCommand const&) {
            output.success(coral_fans::cfsp::SimPlayerManager::getInstance().listGroup());
        }
    );

    // sp list p
    spCommand.runtimeOverload().text("list").text("p").execute(
        [](CommandOrigin const&, CommandOutput& output, ll::command::RuntimeCommand const&) {
            output.success(coral_fans::cfsp::SimPlayerManager::getInstance().listSimPlayer());
        }
    );

    // sp g <name: string> create
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("create")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            if (!player->isSimulatedPlayer()) {
                auto rst = SimPlayerManager::getInstance().createGroup(
                    player,
                    self["name"].get<ll::command::ParamKind::SoftEnum>()
                );
                if (rst.second) output.success(rst.first);
                else output.error(rst.first);
            }
        });

    // sp g <name: string> delete
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("delete")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().deleteGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp g <name: string> addsp <simplayer: string>
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("addsp")
        .required("simplayer", ll::command::ParamKind::SoftEnum, "spname")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().addSpToGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["simplayer"].get<ll::command::ParamKind::SoftEnum>()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp g <name: string> rmsp <simplayer: string>
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("rmsp")
        .required("simplayer", ll::command::ParamKind::SoftEnum, "spname")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().rmSpFromGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["simplayer"].get<ll::command::ParamKind::SoftEnum>()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp g <name: string> addadmin <player: Player>
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("addadmin")
        .required("player", ll::command::ParamKind::Player)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST

            auto targetPlayer = self["player"].get<ll::command::ParamKind::Player>().results(origin).data;
            if (targetPlayer->size() > 1) return output.error("command.sp.error.toomanyobj"_tr());
            if (targetPlayer->front()->isSimulatedPlayer()) return output.error("command.sp.error.nosp"_tr());
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().addAdminToGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["player"].get<ll::command::ParamKind::Player>().results(origin).data->front()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp g <name: string> rmadmin <player: Player>
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("rmadmin")
        .required("player", ll::command::ParamKind::Player)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto targetPlayer = self["player"].get<ll::command::ParamKind::Player>().results(origin).data;
            if (targetPlayer->size() != 1) return output.error("command.sp.error.toomanyobj"_tr());
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().rmAdminFromGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                targetPlayer->front()->getUuid().asString()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp p <name: string> spawn <pos: x y z> <dim: int> <rotx: float> <roty: float>
    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("spawn")
        .optional("pos", ll::command::ParamKind::Vec3)
        .optional("dim", ll::command::ParamKind::Dimension)
        .optional("rotx", ll::command::ParamKind::Float)
        .optional("roty", ll::command::ParamKind::Float)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            if (entity == nullptr) {
                if (!self["pos"].has_value()) {
                    auto& name = self["name"].get<ll::command::ParamKind::SoftEnum>();
                    if (SimPlayerManager::getInstance().fetchSimPlayer(name).has_value()) {
                        SimPlayerManager::getInstance().spawnSimPlayer(nullptr, name, {}, 0, {});
                        return;
                    }
                } else if (self["dim"].has_value()) {
                    Vec2  rot;
                    auto& tem3 = self["rotx"];
                    if (tem3.has_value()) {
                        rot.x      = tem3.get<ll::command::ParamKind::Float>();
                        auto& tem4 = self["roty"];
                        if (tem4.has_value()) rot.y = tem4.get<ll::command::ParamKind::Float>();
                        else rot.y = 0;
                    } else rot = Vec2(0, 0);
                    auto rst = SimPlayerManager::getInstance().spawnSimPlayer(
                        nullptr,
                        self["name"].get<ll::command::ParamKind::SoftEnum>(),
                        self["pos"]
                            .get<ll::command::ParamKind::Vec3>()
                            .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0}),
                        self["dim"].get<ll::command::ParamKind::Dimension>().id,
                        rot
                    );
                    if (rst.second) output.success(rst.first);
                    else output.error(rst.first);
                    return;
                }
                output.error("command.sp.error.paraerror"_tr());
                return;
            } else if (!entity->isType(ActorType ::Player)) {
                output.error("command.sp.error.onlyplayer"_tr());
                return;
            }
            auto* player = static_cast<Player*>(entity);
            COMMAND_SIMPLAYER_CHECKPERMLIST
            Vec3  pos;
            int   dim;
            Vec2  rot;
            auto& tem1 = self["pos"];
            if (tem1.has_value()) {
                pos = tem1.get<ll::command::ParamKind::Vec3>()
                          .getPosition(CommandVersion::CurrentVersion(), origin, {0, 0, 0});
                auto& tem2 = self["dim"];
                if (tem2.has_value()) {
                    dim        = tem2.get<ll::command::ParamKind::Dimension>().id;
                    auto& tem3 = self["rotx"];
                    if (tem3.has_value()) {
                        rot.x      = tem3.get<ll::command::ParamKind::Float>();
                        auto& tem4 = self["roty"];
                        if (tem4.has_value()) rot.y = tem4.get<ll::command::ParamKind::Float>();
                        else rot.y = 0;
                    } else rot = *player->mBuiltInComponents->mActorRotationComponent->mRotationDegree;
                } else {
                    dim = player->getDimensionId();
                    rot = *player->mBuiltInComponents->mActorRotationComponent->mRotationDegree;
                }
            } else {
                const auto& hit = player->traceRay(5.25f);
                if (hit.mType == HitResultType::Entity) pos = hit.getEntity()->getFeetPos();
                else if (hit.mType == HitResultType::Tile) pos = hit.mPos;
                else pos = player->getFeetPos();
                dim = player->getDimensionId();
                rot = *player->mBuiltInComponents->mActorRotationComponent->mRotationDegree;
            }

            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance()
                           .spawnSimPlayer(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), pos, dim, rot);
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp g <name: string> spawn
    spCommand.runtimeOverload()
        .text("g")
        .required("name", ll::command::ParamKind::SoftEnum, "gname")
        .text("spawn")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().spawnGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });


    // sp p <name: string> sneaking
    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("sneaking")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance()
                           .simPlayerSneaking(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp p <name: string> swimming
    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("swimming")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance()
                           .simPlayerSwimming(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp p <name: string> flying
    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("flying")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance()
                           .simPlayerFlying(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    // sp p <name: string> sprinting
    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("sprinting")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance()
                           .simPlayerSprinting(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    ::regSubCmd2(
        spCommand,
        "despawn",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .despawnSimPlayer(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().despawnGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "rm",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .rmSimPlayer(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().rmGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    ::regSubCmd2(
        spCommand,
        "respawn",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .respawnSimPlayer(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().respawnGroup(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "stop",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerStop(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupStop(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 1> enableArg{
        std::make_pair("enable", ll::command::ParamKind::Bool)
    };

    ::regSubCmd(
        spCommand,
        "sneaking",
        enableArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerSneaking(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupSneaking(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "swimming",
        enableArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerSwimming(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupSwimming(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "flying",
        enableArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerFlying(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupFlying(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "sprinting",
        enableArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerSprinting(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupSprinting(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["enable"].get<ll::command::ParamKind::Bool>()
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 2> taskArg{
        std::make_pair("interval", ll::command::ParamKind::Int),
        std::make_pair("times", ll::command::ParamKind::Int)
    };

    ::regSubCmd(
        spCommand,
        "attack",
        {},
        taskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerAttack(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupAttack(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 1> strArg{
        std::make_pair("str", ll::command::ParamKind::String)
    };

    ::regSubCmd(
        spCommand,
        "chat",
        strArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerChat(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["str"].get<ll::command::ParamKind::String>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupChat(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["str"].get<ll::command::ParamKind::String>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "destroy",
        {},
        taskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerDestroy(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupDestroy(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "drop",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerDropSelectedItem(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupDropSelectedItem(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "dropinv",
        {},
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerDropInv(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false);
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupDropInv(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
        }
    );

    spCommand.runtimeOverload()
        .text("p")
        .required("name", ll::command::ParamKind::SoftEnum, "spname")
        .text("swap")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            COMMAND_CHECK_PLAYER
            COMMAND_SIMPLAYER_CHECKPERMLIST
            auto rst = coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerSwap(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>()
            );
            if (rst.second) output.success(rst.first);
            else output.error(rst.first);
        });

    ::regSubCmd(
        spCommand,
        "runcmd",
        strArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerRunCmd(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["str"].get<ll::command::ParamKind::String>()
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupRunCmd(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["str"].get<ll::command::ParamKind::String>()
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 1> itemArg{
        std::make_pair("item", ll::command::ParamKind::Item)
    };

    ::regSubCmd(
        spCommand,
        "select",
        itemArg,
        {},
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerSelect(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["item"].get<ll::command::ParamKind::Item>().mId
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupSelect(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["item"].get<ll::command::ParamKind::Item>().mId
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "interact",
        {},
        taskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerInteract(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupInteract(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "jump",
        {},
        taskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerJump(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupJump(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 3> useTaskArg{
        std::make_pair("tick", ll::command::ParamKind::Int),
        std::make_pair("interval", ll::command::ParamKind::Int),
        std::make_pair("times", ll::command::ParamKind::Int)
    };

    ::regSubCmd(
        spCommand,
        "use",
        {},
        useTaskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerUse(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["tick"].has_value() ? self["tick"].get<ll::command::ParamKind::Int>() : 10,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupUse(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["tick"].has_value() ? self["tick"].get<ll::command::ParamKind::Int>() : 10,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    ::regSubCmd(
        spCommand,
        "build",
        {},
        taskArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerBuild(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupBuild(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["times"].has_value() ? self["times"].get<ll::command::ParamKind::Int>() : 1
            );
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 1> posArg{
        std::make_pair("pos", ll::command::ParamKind::Vec3)
    };

    ::regSubCmd(
        spCommand,
        "lookat",
        {},
        posArg,
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f);
                if (hit.mType == HitResultType::Entity) pos = hit.getEntity()->getPosition();
                else if (hit.mType == HitResultType::Tile) pos = hit.mPos;
                else pos = player->getFeetPos();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerLookAt(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false, pos);
        },
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f);
                if (hit.mType == HitResultType::Entity) pos = hit.getEntity()->getPosition();
                else if (hit.mType == HitResultType::Tile) pos = hit.mPos;
                else pos = player->getPosition();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .groupLookAt(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), pos);
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 2> posArgWithDim{
        std::make_pair("pos", ll::command::ParamKind::Vec3),
        std::make_pair("dim", ll::command::ParamKind::Dimension)
    };

    if (mod().getConfig().simPlayer.allowTp)
        ::regSubCmd(
            spCommand,
            "tp",
            {},
            posArgWithDim,
            [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
                auto* entity = origin.getEntity();
                auto* player = static_cast<Player*>(entity);
                Vec3  pos;
                int   dimId;
                if (self["pos"].has_value())
                    pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                        CommandVersion::CurrentVersion(),
                        origin,
                        {0, 0, 0}
                    );
                else {
                    const auto& hit = player->traceRay(5.25f);
                    if (hit.mType == HitResultType::Entity) pos = hit.getEntity()->getFeetPos();
                    else if (hit.mType == HitResultType::Tile) pos = hit.mPos;
                    else pos = player->getFeetPos();
                }
                if (self["dim"].has_value()) dimId = self["dim"].get<ll::command::ParamKind::Dimension>().id;
                else dimId = player->getDimensionId();
                return coral_fans::cfsp::SimPlayerManager::getInstance()
                    .simPlayerTp(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false, pos, dimId);
            },
            [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
                auto* entity = origin.getEntity();
                auto* player = static_cast<Player*>(entity);
                Vec3  pos;
                int   dimId;
                if (self["pos"].has_value())
                    pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                        CommandVersion::CurrentVersion(),
                        origin,
                        {0, 0, 0}
                    );
                else {
                    const auto& hit = player->traceRay(5.25f);
                    if (hit.mType == HitResultType::Entity) pos = hit.getEntity()->getFeetPos();
                    else if (hit.mType == HitResultType::Tile) pos = hit.mPos;
                    else pos = player->getFeetPos();
                }
                if (self["dim"].has_value()) dimId = self["dim"].get<ll::command::ParamKind::Dimension>().id;
                else dimId = player->getDimensionId();
                return coral_fans::cfsp::SimPlayerManager::getInstance()
                    .groupTp(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), pos, dimId);
            }
        );

    ::regSubCmd(
        spCommand,
        "moveto",
        {},
        posArg,
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f, false, true);
                if (hit) pos = hit.mPos;
                else pos = player->getFeetPos();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerMoveTo(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false, pos);
        },
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f, false, true);
                if (hit) pos = hit.mPos;
                else pos = player->getFeetPos();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .groupMoveTo(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), pos);
        }
    );

    ::regSubCmd(
        spCommand,
        "navto",
        {},
        posArg,
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f, false, true);
                if (hit) pos = hit.mPos;
                else pos = player->getFeetPos();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .simPlayerNavTo(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), false, pos);
        },
        [](CommandOrigin const& origin, ll::command::RuntimeCommand const& self) {
            auto* entity = origin.getEntity();
            auto* player = static_cast<Player*>(entity);
            Vec3  pos;
            if (self["pos"].has_value())
                pos = self["pos"].get<ll::command::ParamKind::Vec3>().getPosition(
                    CommandVersion::CurrentVersion(),
                    origin,
                    {0, 0, 0}
                );
            else {
                const auto& hit = player->traceRay(5.25f, false, true);
                if (hit) pos = hit.mPos;
                else pos = player->getFeetPos();
            }
            return coral_fans::cfsp::SimPlayerManager::getInstance()
                .groupNavTo(player, self["name"].get<ll::command::ParamKind::SoftEnum>(), pos);
        }
    );

    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 1> pathArg{
        std::make_pair("path", ll::command::ParamKind::FilePath)
    };
    std::array<std::pair<std::string, ll::command::ParamKind::Kind>, 2> intervalArgArg{
        std::make_pair("interval", ll::command::ParamKind::Int),
        std::make_pair("arg", ll::command::ParamKind::RawText)
    };

    ::regSubCmd(
        spCommand,
        "script",
        pathArg,
        intervalArgArg,
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().simPlayerScript(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                false,
                self["path"].get<ll::command::ParamKind::FilePath>().mText,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["arg"].has_value() ? self["arg"].get<ll::command::ParamKind::RawText>().mText : ""
            );
        },
        [](Player* player, ll::command::RuntimeCommand const& self) {
            return coral_fans::cfsp::SimPlayerManager::getInstance().groupScript(
                player,
                self["name"].get<ll::command::ParamKind::SoftEnum>(),
                self["path"].get<ll::command::ParamKind::FilePath>().mText,
                self["interval"].has_value() ? self["interval"].get<ll::command::ParamKind::Int>() : 20,
                self["arg"].has_value() ? self["arg"].get<ll::command::ParamKind::RawText>().mText : ""
            );
        }
    );
}

} // namespace coral_fans::cfsp::commands