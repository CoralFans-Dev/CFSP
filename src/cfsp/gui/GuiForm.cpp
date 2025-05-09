#include "GuiForm.h"
#include "FormUI.h"
#include "cfsp/base/Mod.h"
#include "cfsp/simplayer/CFSP.h"
#include "ll/api/i18n/I18n.h"
#include "mc/network/packet/TextPacket.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/world/actor/ActorFlags.h"
#include "mc/world/level/Level.h"
#include <vector>


namespace coral_fans::cfsp::gui {

void sendSpOperatorList(Player*, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo>);
void sendNewSpPage(Player* pl, int = 0, std::string = "", std::string = "");

std::string getGameModeStr(int gameType) {
    using ll::i18n_literals::operator""_tr;
    switch (gameType) {
    case 0:
        return "base.gamemode.survival"_tr();
    case 1:
        return "base.gamemode.creative"_tr();
    case 2:
        return "base.gamemode.adventure"_tr();
    case 6:
        return "base.gamemode.spectator"_tr();
    default:
        return "base.gamemode.undefined"_tr();
    }
}

std::string getSpStatus(int status) {
    using ll::i18n_literals::operator""_tr;
    switch (status) {
    case SimPlayerManager::SimPlayerStatus::Offline:
        return "base.spstatus.offline"_tr();
    case SimPlayerManager::SimPlayerStatus::Alive:
        return "base.spstatus.alive"_tr();
    case SimPlayerManager::SimPlayerStatus::Dead:
        return "base.spstatus.dead"_tr();
    default:
        return "base.spstatus.undefined"_tr();
    }
}

std::string getDimName(int dim) {
    using ll::i18n_literals::operator""_tr;
    switch (dim) {
    case 0:
        return "base.dimension.overworld"_tr();
    case 1:
        return "base.dimension.nether"_tr();
    case 2:
        return "base.dimension.theend"_tr();
    default:
        return "base.dimension.undefined"_tr();
    }
}

void sendAllSplist(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.splist.title"_tr(), "");
    for (auto i : SimPlayerManager::getInstance().getAllSplist()) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto spInfo = SimPlayerManager::getInstance().fetchSimPlayer(i);
            if (spInfo.has_value()) sendSpOperatorList(player, spInfo.value());
        });
    }
    menu.addButton("gui.splist.newsp"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendNewSpPage(player);
    });
    menu.sendTo(pl, [](Player* player, int, lse::form::FormCancelReason) { sendManagerMainMenu(player); });
}

void sendSpInfo(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    std::string content = "\n  " + "gui.spinfo.name"_tr() + spInfo->name + "\n  ";
    content +=
        "gui.spinfo.owner"_tr() + *ll::service::getLevel()->getPlayer(mce::UUID(spInfo->ownerUuid))->mName + "\n  ";
    content += "gui.spinfo.status"_tr() + getSpStatus(spInfo->status) + "\n  ";
    if (spInfo->simPlayer) {
        content += "gui.spinfo.pos"_tr() + getDimName(spInfo->simPlayer->getDimensionId()) + " "
                 + spInfo->simPlayer->getPosition().toJsonString() + "\n  ";
        content += "gui.spinfo.respawnpos"_tr() + getDimName(spInfo->simPlayer->mPlayerRespawnPoint->mDimension->id)
                 + " " + spInfo->simPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        content += "gui.spinfo.gamemode"_tr() + getGameModeStr((int)spInfo->simPlayer->getPlayerGameType()) + "\n  ";
        content += "gui.spinfo.isFree"_tr() + (spInfo->isFree() ? "base.yesno.yes"_tr() : "base.yesno.no"_tr());
    } else {
        content += "gui.spinfo.offlinepos"_tr() + getDimName(spInfo->offlineDim) + " "
                 + Vec3(spInfo->offlinePosX, spInfo->offlinePosY, spInfo->offlinePosZ).toJsonString();
    }
    lse::form::SimpleForm("gui.spinfo.title"_tr(), content)
        .sendTo(pl, [spInfo](Player* player, int, lse::form::FormCancelReason) { sendSpOperatorList(player, spInfo); });
}

void sendInvOperatorList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.invoperatelist.title"_tr(), "");
    menu.addButton("gui.invoperatelist.swap"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().simPlayerSwap(player, spInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.invoperatelist.drop"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().simPlayerDropSelectedItem(player, spInfo->name, false);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.invoperatelist.dropinv"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().simPlayerDropInv(player, spInfo->name, false);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.sendTo(pl);
}

void sendMoveOperator(
    Player*                                                              pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo,
    int                                                                  defDim = 0,
    std::string                                                          defPos = "",
    int                                                                  defOp  = 0
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.moveoperate.title"_tr());
    if (mod().getConfig().simPlayer.allowTp) {
        menu.addDropdown(
            "operate",
            "gui.moveoperate.operate"_tr(),
            std::vector<std::string>{
                "gui.moveoperate.tp"_tr(),
                "gui.moveoperate.moveto"_tr(),
                "gui.moveoperate.navto"_tr()
            },
            defOp
        );
        menu.addInput("pos", "gui.moveoperate.targetpos"_tr(), "0 0 0", defPos);
        menu.addDropdown(
            "dim",
            "gui.moveoperate.targetdim"_tr(),
            std::vector<std::string>{
                "base.dimension.overworld"_tr(),
                "base.dimension.nether"_tr(),
                "base.dimension.theend"_tr()
            },
            defDim
        );
    } else {
        menu.addDropdown(
            "operate",
            "gui.moveoperate.operate"_tr(),
            std::vector<std::string>{"gui.moveoperate.moveto"_tr(), "gui.moveoperate.navto"_tr()},
            defOp
        );
        menu.addInput("pos", "gui.moveoperate.targetpos"_tr(), "0 0 0", defPos);
    }
    menu.sendTo(
        pl,
        [spInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
            else {
                auto const& eleOp  = elements["operate"]->value;
                auto const& elePos = elements["pos"]->value;

                Vec3 targetPos;
                if (elePos == "") targetPos = player->getFeetPos();
                else {
                    std::istringstream iss(elePos);
                    float              x, y, z;
                    char               test;
                    if (!(iss >> x >> y >> z) || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.moveoperate.poserror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (mod().getConfig().simPlayer.allowTp) {
                            if (eleOp == "gui.moveoperate.tp"_tr()) op = 0;
                            else if (eleOp == "gui.moveoperate.moveto"_tr()) op = 1;
                            else if (eleOp == "gui.moveoperate.navto"_tr()) op = 2;
                        } else {
                            if (eleOp == "gui.moveoperate.moveto"_tr()) op = 0;
                            else if (eleOp == "gui.moveoperate.navto"_tr()) op = 1;
                        }

                        int dim = 0;
                        if (mod().getConfig().simPlayer.allowTp) {
                            auto const& eleDim = elements["dim"];
                            if (eleDim->value == "base.dimension.overworld"_tr()) dim = 0;
                            else if (eleDim->value == "base.dimension.nether"_tr()) dim = 1;
                            else if (eleDim->value == "base.dimension.theend"_tr()) dim = 2;
                        }

                        sendMoveOperator(player, spInfo, dim, elePos, op);
                        return;
                    }
                    targetPos = Vec3(x, y, z);
                }

                std::pair<std::string, bool> res;
                if (eleOp == "gui.moveoperate.tp"_tr()) {
                    int         dim    = 0;
                    auto const& eleDim = elements["dim"]->value;
                    if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
                    else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
                    else if (eleDim == "base.dimension.theend"_tr()) dim = 2;

                    res = SimPlayerManager::getInstance().simPlayerTp(player, spInfo->name, false, targetPos, dim);
                } else if (eleOp == "gui.moveoperate.moveto"_tr())
                    res = SimPlayerManager::getInstance().simPlayerMoveTo(player, spInfo->name, false, targetPos);
                else if (eleOp == "gui.moveoperate.navto"_tr())
                    res = SimPlayerManager::getInstance().simPlayerNavTo(player, spInfo->name, false, targetPos);

                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendlookToOperator(
    Player*                                                              pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo,
    std::string                                                          defPos = ""
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.lookatoperate.title"_tr());
    menu.addInput("pos", "gui.lookatoperate.targetpos"_tr(), "0 0 0", defPos);
    menu.sendTo(
        pl,
        [spInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
            else {
                auto const& elePos = elements["pos"]->value;

                Vec3 targetPos;
                if (elePos == "") targetPos = player->getFeetPos();
                else {
                    std::istringstream iss(elePos);
                    float              x, y, z;
                    char               test;
                    if (!(iss >> x >> y >> z) || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.lookatoperate.poserror"_tr());
                        player->sendNetworkPacket(pkt);

                        sendlookToOperator(player, spInfo, elePos);
                        return;
                    }
                    targetPos = Vec3(x, y, z);
                }
                auto res = SimPlayerManager::getInstance().simPlayerLookAt(player, spInfo->name, false, targetPos);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendOperator(
    Player*                                                              pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo,
    int                                                                  defOp       = 0,
    std::string                                                          defInterval = "",
    std::string                                                          defTimes    = "",
    std::string                                                          defLong     = ""
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.operate.title"_tr());
    menu.addDropdown(
        "operate",
        "gui.operate.operate"_tr(),
        std::vector<std::string>{
            "gui.operate.attack"_tr(),
            "gui.operate.interact"_tr(),
            "gui.operate.build"_tr(),
            "gui.operate.use"_tr(),
            "gui.operate.destroy"_tr(),
            "gui.operate.jump"_tr()
        },
        defOp
    );
    menu.addInput("interval", "gui.operate.interval"_tr(), "1", defInterval);
    menu.addInput("times", "gui.operate.times"_tr(), "1", defTimes);
    menu.addInput("long", "gui.operate.long"_tr(), "1", defLong);
    menu.sendTo(
        pl,
        [spInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
            else {
                auto const& eleOp       = elements["operate"]->value;
                auto const& eleInterval = elements["interval"]->value;
                auto const& eleTimes    = elements["times"]->value;
                auto const& eleLong     = elements["long"]->value;
                int         interval = 1, times = 1, _long = 1;
                if (eleInterval != "") {
                    std::istringstream iss(eleInterval);
                    char               test;
                    if (!iss >> interval || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.operate.intervalerror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (eleOp == "gui.operate.attack"_tr()) op = 0;
                        else if (eleOp == "gui.operate.interact"_tr()) op = 1;
                        else if (eleOp == "gui.operate.build"_tr()) op = 2;
                        else if (eleOp == "gui.operate.use"_tr()) op = 3;
                        else if (eleOp == "gui.operate.destroy"_tr()) op = 4;
                        else if (eleOp == "gui.operate.jump"_tr()) op = 5;
                        sendOperator(player, spInfo, op, eleInterval, eleTimes, eleLong);
                        return;
                    }
                }
                if (eleTimes != "") {
                    std::istringstream iss(eleTimes);
                    char               test;
                    if (!iss >> times || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.operate.timeserror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (eleOp == "gui.operate.attack"_tr()) op = 0;
                        else if (eleOp == "gui.operate.interact"_tr()) op = 1;
                        else if (eleOp == "gui.operate.build"_tr()) op = 2;
                        else if (eleOp == "gui.operate.use"_tr()) op = 3;
                        else if (eleOp == "gui.operate.destroy"_tr()) op = 4;
                        else if (eleOp == "gui.operate.jump"_tr()) op = 5;
                        sendOperator(player, spInfo, op, eleInterval, eleTimes, eleLong);
                        return;
                    }
                }
                std::pair<std::string, bool> res;
                if (eleOp == "gui.operate.attack"_tr())
                    res = SimPlayerManager::getInstance().simPlayerAttack(player, spInfo->name, false, interval, times);
                else if (eleOp == "gui.operate.interact"_tr())
                    res =
                        SimPlayerManager::getInstance().simPlayerInteract(player, spInfo->name, false, interval, times);
                else if (eleOp == "gui.operate.build"_tr())
                    res = SimPlayerManager::getInstance().simPlayerBuild(player, spInfo->name, false, interval, times);
                else if (eleOp == "gui.operate.use"_tr()) {
                    if (eleLong != "") {
                        std::istringstream iss(eleLong);
                        char               test;
                        if (!iss >> _long || iss >> test) {
                            TextPacket pkt = TextPacket();
                            pkt.mType      = TextPacketType::Raw;
                            pkt.mMessage.assign("gui.operate.longerror"_tr());
                            player->sendNetworkPacket(pkt);

                            int op = 0;
                            if (eleOp == "gui.operate.attack"_tr()) op = 0;
                            else if (eleOp == "gui.operate.interact"_tr()) op = 1;
                            else if (eleOp == "gui.operate.build"_tr()) op = 2;
                            else if (eleOp == "gui.operate.use"_tr()) op = 3;
                            else if (eleOp == "gui.operate.destroy"_tr()) op = 4;
                            else if (eleOp == "gui.operate.jump"_tr()) op = 5;
                            sendOperator(player, spInfo, op, eleInterval, eleTimes, eleLong);
                            return;
                        }
                    }
                    res = SimPlayerManager::getInstance()
                              .simPlayerUse(player, spInfo->name, false, _long, interval, times);
                } else if (eleOp == "gui.operate.destroy"_tr())
                    res =
                        SimPlayerManager::getInstance().simPlayerDestroy(player, spInfo->name, false, interval, times);
                else if (eleOp == "gui.operate.jump"_tr())
                    res = SimPlayerManager::getInstance().simPlayerJump(player, spInfo->name, false, interval, times);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendActionOperator(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.actionoperate.title"_tr());
    menu.addDropdown(
        "sneaking",
        "gui.actionoperate.sneaking"_tr(),
        std::vector<std::string>{"gui.actionoperate.sneak"_tr(), "gui.actionoperate.releasesneak"_tr()},
        spInfo->simPlayer && spInfo->simPlayer->getStatusFlag(ActorFlags::Sneaking) ? 0 : 1
    );
    menu.addDropdown(
        "swimming",
        "gui.actionoperate.swimming"_tr(),
        std::vector<std::string>{"gui.actionoperate.swim"_tr(), "gui.actionoperate.releaseswim"_tr()},
        spInfo->simPlayer && spInfo->simPlayer->getStatusFlag(ActorFlags::Swimming) ? 0 : 1
    );
    menu.sendTo(
        pl,
        [spInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
            else {
                std::pair<std::string, bool> res;

                auto& sneaking = elements["sneaking"]->value;
                if (sneaking == "gui.actionoperate.sneak"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSneaking(player, spInfo->name, false, true);
                else if (sneaking == "gui.actionoperate.releasesneak"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSneaking(player, spInfo->name, false, false);

                auto& swimming = elements["swimming"]->value;
                if (swimming == "gui.actionoperate.swim"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSwimming(player, spInfo->name, false, true);
                else if (swimming == "gui.actionoperate.releaseswim"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSwimming(player, spInfo->name, false, false);

                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendMessageOperator(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.messageoperate.title"_tr());
    menu.addDropdown(
        "operate",
        "gui.messageoperate.operate"_tr(),
        std::vector<std::string>{"gui.messageoperate.chat"_tr(), "gui.messageoperate.runcmd"_tr()}
    );
    menu.addInput("text", "gui.messageoperate.text"_tr());
    menu.sendTo(
        pl,
        [spInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
            else {
                std::pair<std::string, bool> res;

                auto& op = elements["operate"]->value;
                if (op == "gui.messageoperate.chat"_tr())
                    res = SimPlayerManager::getInstance()
                              .simPlayerChat(player, spInfo->name, false, elements["text"]->value);
                else if (op == "gui.messageoperate.runcmd"_tr())
                    res = SimPlayerManager::getInstance()
                              .simPlayerRunCmd(player, spInfo->name, false, elements["text"]->value);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendSpOperatorList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.operatelist.title"_tr(), "");
    if (spInfo) {
        menu.addButton("gui.operatelist.spinfo"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
            sendSpInfo(player, spInfo);
        });
        if (spInfo->status == SimPlayerManager::SimPlayerStatus::Offline) {
            menu.addButton("gui.operatelist.online"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                auto       res = SimPlayerManager::getInstance().spawnSimPlayer(player, spInfo->name, {}, {});
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            });
            menu.addButton("gui.operatelist.rm"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                auto       res = SimPlayerManager::getInstance().rmSimPlayer(player, spInfo->name, false);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            });
        } else
            menu.addButton("gui.operatelist.offline"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                auto       res = SimPlayerManager::getInstance().despawnSimPlayer(player, spInfo->name, false);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            });
        if (spInfo->status == SimPlayerManager::SimPlayerStatus::Dead)
            menu.addButton("gui.operatelist.respawn"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                auto       res = SimPlayerManager::getInstance().respawnSimPlayer(player, spInfo->name, false);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            });
        if (spInfo->status == SimPlayerManager::SimPlayerStatus::Alive) {
            menu.addButton("gui.operatelist.inv"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                sendInvOperatorList(player, spInfo);
            });
            menu.addButton("gui.operatelist.move"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                sendMoveOperator(player, spInfo, (int)player->getDimensionId());
            });
            menu.addButton("gui.operatelist.lookat"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                sendlookToOperator(player, spInfo);
            });
            if (spInfo->isFree())
                menu.addButton(
                    "gui.operatelist.operate"_tr(),
                    "",
                    [spInfo](Player* player, lse::form::FormCancelReason) { sendOperator(player, spInfo); }
                );
            else
                menu.addButton("gui.operatelist.stop"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                    auto       res = SimPlayerManager::getInstance().simPlayerStop(player, spInfo->name, false);
                    TextPacket pkt = TextPacket();
                    pkt.mType      = TextPacketType::Raw;
                    if (res.second) pkt.mMessage.assign(res.first);
                    else pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                });
            menu.addButton("gui.operatelist.action"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                sendActionOperator(player, spInfo);
            });
            menu.addButton("gui.operatelist.message"_tr(), "", [spInfo](Player* player, lse::form::FormCancelReason) {
                sendMessageOperator(player, spInfo);
            });
        }
    }
    menu.sendTo(pl);
}

void sendNewSpPage(Player* pl, int defDim, std::string defName, std::string defPos) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.createsp.title"_tr());
    menu.addInput("name", "gui.createsp.name"_tr(), "", defName);
    menu.addInput("targetpos", "gui.createsp.targetpos"_tr(), "0 0 0", defPos);
    menu.addDropdown(
        "dim",
        "gui.createsp.targetdim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
    menu.sendTo(
        pl,
        [](Player*                                                              player,
           std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
           lse::form::FormCancelReason                                          cancelReason) {
            if (!cancelReason.has_value()) {
                auto& eleName = elements["name"]->value;
                auto& elePos  = elements["targetpos"]->value;

                int         dim    = 0;
                auto const& eleDim = elements["dim"]->value;
                if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
                else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
                else if (eleDim == "base.dimension.theend"_tr()) dim = 2;

                Vec3 targetPos;
                if (elePos == "") targetPos = player->getFeetPos();
                else {
                    std::istringstream iss(elePos);
                    float              x, y, z;
                    char               test;
                    if (!(iss >> x >> y >> z) || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.createsp.poserror"_tr());
                        player->sendNetworkPacket(pkt);

                        sendNewSpPage(player, dim, eleName, elePos);
                        return;
                    }

                    targetPos = Vec3(x, y, z);
                }
                auto res = SimPlayerManager::getInstance().spawnSimPlayer(
                    player,
                    eleName,
                    targetPos,
                    *player->mBuiltInComponents->mActorRotationComponent->mRotationDegree
                );
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendGroupInfo(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    std::string content = "\n  " + "gui.spinfo.name"_tr() + spInfo->name + "\n  ";
    content +=
        "gui.spinfo.owner"_tr() + *ll::service::getLevel()->getPlayer(mce::UUID(spInfo->ownerUuid))->mName + "\n  ";
    content += "gui.spinfo.status"_tr() + getSpStatus(spInfo->status) + "\n  ";
    if (spInfo->simPlayer) {
        content += "gui.spinfo.pos"_tr() + getDimName(spInfo->simPlayer->getDimensionId()) + " "
                 + spInfo->simPlayer->getPosition().toJsonString() + "\n  ";
        content += "gui.spinfo.respawnpos"_tr() + getDimName(spInfo->simPlayer->mPlayerRespawnPoint->mDimension->id)
                 + " " + spInfo->simPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        content += "gui.spinfo.gamemode"_tr() + getGameModeStr((int)spInfo->simPlayer->getPlayerGameType()) + "\n  ";
        content += "gui.spinfo.isFree"_tr() + (spInfo->isFree() ? "base.yesno.yes"_tr() : "base.yesno.no"_tr());
    } else {
        content += "gui.spinfo.offlinepos"_tr() + getDimName(spInfo->offlineDim) + " "
                 + Vec3(spInfo->offlinePosX, spInfo->offlinePosY, spInfo->offlinePosZ).toJsonString();
    }
    lse::form::SimpleForm("gui.spinfo.title"_tr(), content)
        .sendTo(pl, [spInfo](Player* player, int, lse::form::FormCancelReason) { sendSpOperatorList(player, spInfo); });
}

void sendGroupOperatorList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.groupoperatelist.title"_tr(), "");
    menu.addButton("gui.groupoperatelist.info"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.manage"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.online"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.offline"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
    });
    menu.addButton("gui.groupoperatelist.respawn"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
    });
    menu.addButton("gui.groupoperatelist.inv"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.move"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.lookat"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.operate"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
    });
    menu.addButton("gui.groupoperatelist.action"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.addButton("gui.groupoperatelist.message"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
    });
    menu.addButton("gui.groupoperatelist.rm"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {});
    menu.sendTo(pl);
}

void sendAllGroupList(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.grouplist.title"_tr(), "");
    for (auto i : SimPlayerManager::getInstance().getAllGroupList()) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto groupInfo = SimPlayerManager::getInstance().fetchGroup(i);
            if (groupInfo.has_value()) sendGroupOperatorList(player, groupInfo.value());
        });
    }
    menu.addButton("gui.grouplist.newgroup"_tr(), "", [](Player*, lse::form::FormCancelReason) {});
    menu.sendTo(pl);
}

void sendManagerMainMenu(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto mainMenu = lse::form::SimpleForm("gui.mainmenu.title"_tr(), "");
    mainMenu.addButton("gui.mainmenu.splistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendAllSplist(player);
    });
    mainMenu.addButton("gui.mainmenu.grouplistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendAllGroupList(player);
    });
    mainMenu.addButton("gui.mainmenu.managerlistbutton"_tr(), "", [](Player*, lse::form::FormCancelReason) {

    });
    mainMenu.sendTo(pl);
}

void sendMainMenu(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto mainMenu = lse::form::SimpleForm("gui.mainmenu.title"_tr(), "");
    mainMenu.addButton("gui.mainmenu.splistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendSplist(player);
    });
    mainMenu.addButton("gui.mainmenu.grouplistbutton"_tr(), "", [](Player*, lse::form::FormCancelReason) {

    });
    mainMenu.sendTo(pl, [](Player*, int, lse::form::FormCancelReason) {});
}

void sendSplist(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto               menu  = lse::form::SimpleForm("gui.splist.title"_tr(), "");
    unsigned long long count = 0;
    for (auto i : SimPlayerManager::getInstance().fetchSplist(pl)) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto spInfo = SimPlayerManager::getInstance().fetchSimPlayer(i);
            if (spInfo.has_value()) sendSpOperatorList(player, spInfo.value());
        });
        count++;
    }
    if (pl->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
        || mod().getConfig().simPlayer.superManagerList.contains(*pl->mName)
        || count < mod().getConfig().simPlayer.maxOwn)
        menu.addButton("gui.splist.newsp"_tr(), "", [](Player*, lse::form::FormCancelReason) {});
    menu.sendTo(pl, [](Player*, int, lse::form::FormCancelReason) {});
}


} // namespace coral_fans::cfsp::gui