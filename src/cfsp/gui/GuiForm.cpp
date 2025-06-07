#include "GuiForm.h"
#include "FormUI.h"
// #include "cfsp/CFSP.h"
#include "cfsp/base/Mod.h"
#include "cfsp/simplayer/CFSP.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/network/packet/TextPacket.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/world/actor/ActorFlags.h"
#include "mc/world/actor/player/AbilitiesIndex.h"
#include "mc/world/actor/player/Ability.h"
#include "mc/world/actor/provider/ActorAttribute.h"
#include "mc/world/level/Level.h"
#include <string>
#include <utility>
#include <vector>

namespace coral_fans::cfsp::gui {


void sendNewSpPage(Player* pl, int = 0, std::string = "", std::string = "");
void sendGroupManageList(Player*, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo>);

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
    auto menu = lse::form::SimpleForm("gui.managersplist.title"_tr(), "");
    for (auto i : SimPlayerManager::getInstance().getAllSplist()) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto spInfo = SimPlayerManager::getInstance().fetchSimPlayer(i);
            if (spInfo.has_value()) sendSpOperatorList(player, spInfo.value());
        });
    }
    menu.addButton("gui.managersplist.newsp"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendNewSpPage(player);
    });
    menu.sendTo(pl);
}

void sendSpInfo(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo> spInfo) {
    using ll::i18n_literals::operator""_tr;
    std::string content  = "\n  " + "gui.spinfo.name"_tr() + spInfo->name + "\n  ";
    content             += "gui.spinfo.owner"_tr() + utils::tryGetPlayerName(spInfo->ownerUuid) + "\n  ";
    content             += "gui.spinfo.status"_tr() + getSpStatus(spInfo->status) + "\n  ";
    if (spInfo->simPlayer) {
        content += "gui.spinfo.pos"_tr() + getDimName(spInfo->simPlayer->getDimensionId()) + " "
                 + spInfo->simPlayer->getPosition().toJsonString() + "\n  ";
        content += "gui.spinfo.respawnpos"_tr() + getDimName(spInfo->simPlayer->mPlayerRespawnPoint->mDimension->id)
                 + " " + spInfo->simPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        content += "gui.spinfo.gamemode"_tr() + getGameModeStr((int)spInfo->simPlayer->getPlayerGameType()) + "\n  ";
        content += "gui.spinfo.health"_tr()
                 + std::to_string(ActorAttribute::getHealth(spInfo->simPlayer->getEntityContext())) + " / "
                 + std::to_string(spInfo->simPlayer->getMaxHealth()) + "\n  ";
        content += "gui.spinfo.isFree"_tr() + (spInfo->isFree() ? "base.yesno.yes"_tr() : "base.yesno.no"_tr());
    } else {
        content +=
            "gui.spinfo.offlinepos"_tr() + getDimName(spInfo->offlineDim) + " " + spInfo->offlinePos.toJsonString();
    }
    lse::form::SimpleForm("gui.spinfo.title"_tr(), content)
        .sendTo(pl, [spInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) sendSpOperatorList(player, spInfo);
        });
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
    menu.sendTo(pl, [spInfo](Player* player, int, lse::form::FormCancelReason cancelReanson) {
        if (cancelReanson.has_value()) sendSpOperatorList(player, spInfo);
    });
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

void sendLookToOperator(
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

                        sendLookToOperator(player, spInfo, elePos);
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
        "gui.operate.opeate"_tr(),
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
    menu.addInput("interval", "gui.operate.interval"_tr(), "20", defInterval);
    menu.addInput("times", "gui.operate.times"_tr(), "1", defTimes);
    menu.addInput("long", "gui.operate.long"_tr(), "10", defLong);
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
                int         interval = 20, times = 1, _long = 10;
                if (eleInterval != "") {
                    std::istringstream iss(eleInterval);
                    char               test;
                    if (!(iss >> interval) || iss >> test) {
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
                    if (!(iss >> times) || iss >> test) {
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
                        if (!(iss >> _long) || iss >> test) {
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
    if (spInfo->simPlayer) {
        if (spInfo->simPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal) {
            menu.addDropdown(
                "flying",
                "gui.actionoperate.flying"_tr(),
                std::vector<std::string>{"gui.actionoperate.fly"_tr(), "gui.actionoperate.releasefly"_tr()},
                spInfo->simPlayer->isFlying() ? 0 : 1
            );
        }
        menu.addDropdown(
            "sneaking",
            "gui.actionoperate.sneaking"_tr(),
            std::vector<std::string>{"gui.actionoperate.sneak"_tr(), "gui.actionoperate.releasesneak"_tr()},
            spInfo->simPlayer->getStatusFlag(ActorFlags::Sneaking) ? 0 : 1
        );
        menu.addDropdown(
            "swimming",
            "gui.actionoperate.swimming"_tr(),
            std::vector<std::string>{"gui.actionoperate.swim"_tr(), "gui.actionoperate.releaseswim"_tr()},
            spInfo->simPlayer->getStatusFlag(ActorFlags::Swimming) ? 0 : 1
        );
        menu.addDropdown(
            "sprinting",
            "gui.actionoperate.sprinting"_tr(),
            std::vector<std::string>{"gui.actionoperate.sprint"_tr(), "gui.actionoperate.releasesprint"_tr()},
            spInfo->simPlayer->getStatusFlag(ActorFlags::Sprinting) ? 0 : 1
        );
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
                bool                         flag = true;
                std::pair<std::string, bool> res;
                TextPacket                   pkt = TextPacket();
                pkt.mType                        = TextPacketType::Raw;

                if (spInfo->simPlayer->canFly()) {
                    auto& flying = elements["flying"]->value;
                    if (flying == "gui.actionoperate.fly"_tr())
                        res = SimPlayerManager::getInstance().simPlayerFlying(player, spInfo->name, false, true);
                    else if (flying == "gui.actionoperate.releasefly"_tr())
                        res = SimPlayerManager::getInstance().simPlayerFlying(player, spInfo->name, false, false);
                    if (!res.second) {
                        flag = false;
                        pkt.mMessage.assign("§c" + res.first + "§r");
                        player->sendNetworkPacket(pkt);
                    }
                }

                auto& sneaking = elements["sneaking"]->value;
                if (sneaking == "gui.actionoperate.sneak"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSneaking(player, spInfo->name, false, true);
                else if (sneaking == "gui.actionoperate.releasesneak"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSneaking(player, spInfo->name, false, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                auto& swimming = elements["swimming"]->value;
                if (swimming == "gui.actionoperate.swim"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSwimming(player, spInfo->name, false, true);
                else if (swimming == "gui.actionoperate.releaseswim"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSwimming(player, spInfo->name, false, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                auto& sprinting = elements["sprinting"]->value;
                if (sprinting == "gui.actionoperate.sprint"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSprinting(player, spInfo->name, false, true);
                else if (sprinting == "gui.actionoperate.releasesprint"_tr())
                    res = SimPlayerManager::getInstance().simPlayerSprinting(player, spInfo->name, false, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                if (flag) {
                    pkt.mMessage.assign(res.first);
                    player->sendNetworkPacket(pkt);
                }
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
                auto       res = SimPlayerManager::getInstance().spawnSimPlayer(player, spInfo->name, {}, {}, 0);
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
                sendLookToOperator(player, spInfo);
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
                    dim,
                    *player->mBuiltInComponents->mActorRotationComponent->mRotationDegree
                );
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) {
                    pkt.mMessage.assign(res.first);
                    player->sendNetworkPacket(pkt);
                } else {
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                    sendNewSpPage(player, dim, eleName, elePos);
                }
            }
        }
    );
}

void sendGroupInfo(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    std::string content = "gui.groupinfo.name"_tr() + groupInfo->name + "\n  ";
    content += "gui.groupinfo.owner"_tr() + utils::tryGetPlayerName(groupInfo->owner) + "gui.groupinfo.admin"_tr();
    for (auto& i : groupInfo->admin) {
        content += "  " + utils::tryGetPlayerName(i) + "\n  ";
    }
    content += "gui.groupinfo.sp"_tr();
    for (auto& i : groupInfo->splist) {
        content += "\n    " + i;
    }

    lse::form::SimpleForm("gui.groupinfo.title"_tr(), content)
        .sendTo(pl, [groupInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
        });
}

void sendManageSpInGroup(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.managesp.title"_tr());

    std::vector<std::string> splist;
    if (pl->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
        || mod().getConfig().simPlayer.superManagerList.contains(*pl->mName))
        splist = SimPlayerManager::getInstance().getAllSplist();
    else splist = SimPlayerManager::getInstance().fetchSplist(pl);
    for (auto i : splist) {
        if (!groupInfo->splist.contains(i)) menu.addToggle(i, i, groupInfo->splist.contains(i));
    }
    menu.sendTo(
        pl,
        [groupInfo, splist](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupManageList(player, groupInfo);
            else {
                for (auto i : splist) {
                    auto tem = elements[i];
                    if (tem) {
                        if (tem->value == "1" && !groupInfo->splist.contains(i))
                            SimPlayerManager::getInstance().addSpToGroup(player, groupInfo->name, i);
                        else if (tem->value == "0" && groupInfo->splist.contains(i))
                            SimPlayerManager::getInstance().rmSpFromGroup(player, groupInfo->name, i);
                    }
                }

                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                pkt.mMessage.assign("操作成功");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendPlayerListToAddAdmin(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto                 menu = lse::form::SimpleForm("gui.addadmin.title"_tr(), "");
    std::vector<Player*> playerlist;
    auto                 level = ll::service::getLevel();
    if (level)
        level->forEachPlayer([&playerlist](Player& player) {
            if (!player.isSimulatedPlayer()) playerlist.emplace_back(&player);
            return true;
        });
    std::sort(playerlist.begin(), playerlist.end(), [](Player* a, Player* b) { return *a->mName < *b->mName; });
    for (auto i : playerlist) {
        menu.addButton(*i->mName, "", [i, groupInfo](Player* player, lse::form::FormCancelReason) {
            auto       res = SimPlayerManager::getInstance().addAdminToGroup(player, groupInfo->name, i);
            TextPacket pkt = TextPacket();
            pkt.mType      = TextPacketType::Raw;
            if (res.second) pkt.mMessage.assign(res.first);
            else pkt.mMessage.assign("§c" + res.first + "§r");
            player->sendNetworkPacket(pkt);
        });
    }
    menu.sendTo(pl, [groupInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value()) sendGroupManageList(player, groupInfo);
    });
}

void sendGroupAdminListToRmAdmin(
    Player*                                                          pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.rmadmin.title"_tr(), "");
    for (auto i : groupInfo->admin) {
        menu.addButton(utils::tryGetPlayerName(i), "", [i, groupInfo](Player* player, lse::form::FormCancelReason) {
            auto       res = SimPlayerManager::getInstance().rmAdminFromGroup(player, groupInfo->name, i);
            TextPacket pkt = TextPacket();
            pkt.mType      = TextPacketType::Raw;
            if (res.second) pkt.mMessage.assign(res.first);
            else pkt.mMessage.assign("§c" + res.first + "§r");
            player->sendNetworkPacket(pkt);
        });
    }
    menu.sendTo(pl, [groupInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value()) sendGroupManageList(player, groupInfo);
    });
}

void sendGroupManageList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.groupmanage.title"_tr(), "");
    menu.addButton("gui.groupmanage.managesp"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendManageSpInGroup(player, groupInfo);
    });
    menu.addButton("gui.groupmanage.addadmin"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendPlayerListToAddAdmin(player, groupInfo);
    });
    menu.addButton("gui.groupmanage.removeadmin"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupAdminListToRmAdmin(player, groupInfo);
    });
    menu.addButton("gui.groupmanage.delete"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().deleteGroup(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.sendTo(pl, [groupInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
    });
}

void sendGroupInvOperatorList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.groupinvoperatelist.title"_tr(), "");
    menu.addButton("gui.groupinvoperatelist.drop"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().groupDropSelectedItem(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton(
        "gui.groupinvoperatelist.dropinv"_tr(),
        "",
        [groupInfo](Player* player, lse::form::FormCancelReason) {
            auto       res = SimPlayerManager::getInstance().groupDropInv(player, groupInfo->name);
            TextPacket pkt = TextPacket();
            pkt.mType      = TextPacketType::Raw;
            if (res.second) pkt.mMessage.assign(res.first);
            else pkt.mMessage.assign("§c" + res.first + "§r");
            player->sendNetworkPacket(pkt);
        }
    );
    menu.sendTo(pl, [groupInfo](Player* player, int, lse::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
    });
}

void sendGroupMoveOperator(
    Player*                                                          pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo,
    int                                                              defDim = 0,
    std::string                                                      defPos = "",
    int                                                              defOp  = 0
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.groupmoveoperate.title"_tr());
    if (mod().getConfig().simPlayer.allowTp) {
        menu.addDropdown(
            "operate",
            "gui.groupmoveoperate.operate"_tr(),
            std::vector<std::string>{
                "gui.groupmoveoperate.tp"_tr(),
                "gui.groupmoveoperate.moveto"_tr(),
                "gui.groupmoveoperate.navto"_tr()
            },
            defOp
        );
        menu.addInput("pos", "gui.groupmoveoperate.targetpos"_tr(), "0 0 0", defPos);
        menu.addDropdown(
            "dim",
            "gui.groupmoveoperate.targetdim"_tr(),
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
            "gui.groupmoveoperate.operate"_tr(),
            std::vector<std::string>{"gui.groupmoveoperate.moveto"_tr(), "gui.groupmoveoperate.navto"_tr()},
            defOp
        );
        menu.addInput("pos", "gui.groupmoveoperate.targetpos"_tr(), "0 0 0", defPos);
    }
    menu.sendTo(
        pl,
        [groupInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
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
                        pkt.mMessage.assign("gui.groupmoveoperate.poserror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (mod().getConfig().simPlayer.allowTp) {
                            if (eleOp == "gui.groupmoveoperate.tp"_tr()) op = 0;
                            else if (eleOp == "gui.groupmoveoperate.moveto"_tr()) op = 1;
                            else if (eleOp == "gui.groupmoveoperate.navto"_tr()) op = 2;
                        } else {
                            if (eleOp == "gui.groupmoveoperate.moveto"_tr()) op = 0;
                            else if (eleOp == "gui.groupmoveoperate.navto"_tr()) op = 1;
                        }

                        int dim = 0;
                        if (mod().getConfig().simPlayer.allowTp) {
                            auto const& eleDim = elements["dim"];
                            if (eleDim->value == "base.dimension.overworld"_tr()) dim = 0;
                            else if (eleDim->value == "base.dimension.nether"_tr()) dim = 1;
                            else if (eleDim->value == "base.dimension.theend"_tr()) dim = 2;
                        }

                        sendGroupMoveOperator(player, groupInfo, dim, elePos, op);
                        return;
                    }
                    targetPos = Vec3(x, y, z);
                }

                std::pair<std::string, bool> res;
                if (eleOp == "gui.groupmoveoperate.tp"_tr()) {
                    int         dim    = 0;
                    auto const& eleDim = elements["dim"]->value;
                    if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
                    else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
                    else if (eleDim == "base.dimension.theend"_tr()) dim = 2;

                    res = SimPlayerManager::getInstance().groupTp(player, groupInfo->name, targetPos, dim);
                } else if (eleOp == "gui.groupmoveoperate.moveto"_tr())
                    res = SimPlayerManager::getInstance().groupMoveTo(player, groupInfo->name, targetPos);
                else if (eleOp == "gui.groupmoveoperate.navto"_tr())
                    res = SimPlayerManager::getInstance().groupNavTo(player, groupInfo->name, targetPos);

                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendGroupLookToOperator(
    Player*                                                          pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo,
    std::string                                                      defPos = ""
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.grouplookatoperate.title"_tr());
    menu.addInput("pos", "gui.grouplookatoperate.targetpos"_tr(), "0 0 0", defPos);
    menu.sendTo(
        pl,
        [groupInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
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
                        pkt.mMessage.assign("gui.grouplookatoperate.poserror"_tr());
                        player->sendNetworkPacket(pkt);

                        sendGroupLookToOperator(player, groupInfo, elePos);
                        return;
                    }
                    targetPos = Vec3(x, y, z);
                }
                auto       res = SimPlayerManager::getInstance().groupLookAt(player, groupInfo->name, targetPos);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendGroupOperator(
    Player*                                                          pl,
    boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo,
    int                                                              defOp       = 0,
    std::string                                                      defInterval = "",
    std::string                                                      defTimes    = "",
    std::string                                                      defLong     = ""
) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.groupoperate.title"_tr());
    menu.addDropdown(
        "operate",
        "gui.groupoperate.operate"_tr(),
        std::vector<std::string>{
            "gui.groupoperate.attack"_tr(),
            "gui.groupoperate.interact"_tr(),
            "gui.groupoperate.build"_tr(),
            "gui.groupoperate.use"_tr(),
            "gui.groupoperate.destroy"_tr(),
            "gui.groupoperate.jump"_tr()
        },
        defOp
    );
    menu.addInput("interval", "gui.operate.interval"_tr(), "1", defInterval);
    menu.addInput("times", "gui.operate.times"_tr(), "20", defTimes);
    menu.addInput("long", "gui.operate.long"_tr(), "10", defLong);
    menu.sendTo(
        pl,
        [groupInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
            else {
                auto const& eleOp       = elements["operate"]->value;
                auto const& eleInterval = elements["interval"]->value;
                auto const& eleTimes    = elements["times"]->value;
                auto const& eleLong     = elements["long"]->value;
                int         interval = 1, times = 20, _long = 10;
                if (eleInterval != "") {
                    std::istringstream iss(eleInterval);
                    char               test;
                    if (!(iss >> interval) || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.groupoperate.intervalerror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (eleOp == "gui.groupoperate.attack"_tr()) op = 0;
                        else if (eleOp == "gui.groupoperate.interact"_tr()) op = 1;
                        else if (eleOp == "gui.groupoperate.build"_tr()) op = 2;
                        else if (eleOp == "gui.groupoperate.use"_tr()) op = 3;
                        else if (eleOp == "gui.groupoperate.destroy"_tr()) op = 4;
                        else if (eleOp == "gui.groupoperate.jump"_tr()) op = 5;
                        sendGroupOperator(player, groupInfo, op, eleInterval, eleTimes, eleLong);
                        return;
                    }
                }
                if (eleTimes != "") {
                    std::istringstream iss(eleTimes);
                    char               test;
                    if (!(iss >> times) || iss >> test) {
                        TextPacket pkt = TextPacket();
                        pkt.mType      = TextPacketType::Raw;
                        pkt.mMessage.assign("gui.groupoperate.timeserror"_tr());
                        player->sendNetworkPacket(pkt);

                        int op = 0;
                        if (eleOp == "gui.groupoperate.attack"_tr()) op = 0;
                        else if (eleOp == "gui.groupoperate.interact"_tr()) op = 1;
                        else if (eleOp == "gui.groupoperate.build"_tr()) op = 2;
                        else if (eleOp == "gui.groupoperate.use"_tr()) op = 3;
                        else if (eleOp == "gui.groupoperate.destroy"_tr()) op = 4;
                        else if (eleOp == "gui.groupoperate.jump"_tr()) op = 5;
                        sendGroupOperator(player, groupInfo, op, eleInterval, eleTimes, eleLong);
                        return;
                    }
                }
                std::pair<std::string, bool> res;
                if (eleOp == "gui.groupoperate.attack"_tr())
                    res = SimPlayerManager::getInstance().groupAttack(player, groupInfo->name, interval, times);
                else if (eleOp == "gui.groupoperate.interact"_tr())
                    res = SimPlayerManager::getInstance().groupInteract(player, groupInfo->name, interval, times);
                else if (eleOp == "gui.groupoperate.build"_tr())
                    res = SimPlayerManager::getInstance().groupBuild(player, groupInfo->name, interval, times);
                else if (eleOp == "gui.groupoperate.use"_tr()) {
                    if (eleLong != "") {
                        std::istringstream iss(eleLong);
                        char               test;
                        if (!(iss >> _long) || iss >> test) {
                            TextPacket pkt = TextPacket();
                            pkt.mType      = TextPacketType::Raw;
                            pkt.mMessage.assign("gui.groupoperate.longerror"_tr());
                            player->sendNetworkPacket(pkt);

                            int op = 0;
                            if (eleOp == "gui.groupoperate.attack"_tr()) op = 0;
                            else if (eleOp == "gui.groupoperate.interact"_tr()) op = 1;
                            else if (eleOp == "gui.groupoperate.build"_tr()) op = 2;
                            else if (eleOp == "gui.groupoperate.use"_tr()) op = 3;
                            else if (eleOp == "gui.groupoperate.destroy"_tr()) op = 4;
                            else if (eleOp == "gui.groupoperate.jump"_tr()) op = 5;
                            sendGroupOperator(player, groupInfo, op, eleInterval, eleTimes, eleLong);
                            return;
                        }
                    }
                    res = SimPlayerManager::getInstance().groupUse(player, groupInfo->name, _long, interval, times);
                } else if (eleOp == "gui.groupoperate.destroy"_tr())
                    res = SimPlayerManager::getInstance().groupDestroy(player, groupInfo->name, interval, times);
                else if (eleOp == "gui.groupoperate.jump"_tr())
                    res = SimPlayerManager::getInstance().groupJump(player, groupInfo->name, interval, times);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendGroupActionOperator(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.groupactionoperate.title"_tr());
    menu.addDropdown(
        "flying",
        "gui.groupactionoperate.flying"_tr(),
        std::vector<std::string>{
            "gui.groupactionoperate.notoperate"_tr(),
            "gui.groupactionoperate.fly"_tr(),
            "gui.groupactionoperate.releasefly"_tr()
        }
    );
    menu.addDropdown(
        "sneaking",
        "gui.groupactionoperate.sneaking"_tr(),
        std::vector<std::string>{
            "gui.groupactionoperate.notoperate"_tr(),
            "gui.groupactionoperate.sneak"_tr(),
            "gui.groupactionoperate.releasesneak"_tr()
        }
    );
    menu.addDropdown(
        "swimming",
        "gui.groupactionoperate.swimming"_tr(),
        std::vector<std::string>{
            "gui.groupactionoperate.notoperate"_tr(),
            "gui.groupactionoperate.swim"_tr(),
            "gui.groupactionoperate.releaseswim"_tr()
        }
    );
    menu.addDropdown(
        "sprinting",
        "gui.groupactionoperate.sprinting"_tr(),
        std::vector<std::string>{
            "gui.groupactionoperate.notoperate"_tr(),
            "gui.groupactionoperate.sprint"_tr(),
            "gui.groupactionoperate.releasesprint"_tr()
        }
    );
    menu.sendTo(
        pl,
        [groupInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
            else {
                bool                         flag = true;
                std::pair<std::string, bool> res  = std::make_pair("", true);
                TextPacket                   pkt  = TextPacket();
                pkt.mType                         = TextPacketType::Raw;

                auto& flying = elements["flying"]->value;
                if (flying == "gui.groupactionoperate.fly"_tr())
                    res = SimPlayerManager::getInstance().groupFlying(player, groupInfo->name, true);
                else if (flying == "gui.groupactionoperate.releasefly"_tr())
                    res = SimPlayerManager::getInstance().groupFlying(player, groupInfo->name, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                auto& sneaking = elements["sneaking"]->value;
                if (sneaking == "gui.groupactionoperate.sneak"_tr())
                    res = SimPlayerManager::getInstance().groupSneaking(player, groupInfo->name, true);
                else if (sneaking == "gui.groupactionoperate.releasesneak"_tr())
                    res = SimPlayerManager::getInstance().groupSneaking(player, groupInfo->name, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                auto& swimming = elements["swimming"]->value;
                if (swimming == "gui.groupactionoperate.swim"_tr())
                    res = SimPlayerManager::getInstance().groupSwimming(player, groupInfo->name, true);
                else if (swimming == "gui.groupactionoperate.releaseswim"_tr())
                    res = SimPlayerManager::getInstance().groupSwimming(player, groupInfo->name, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                auto& sprinting = elements["sprinting"]->value;
                if (sprinting == "gui.groupactionoperate.sprint"_tr())
                    res = SimPlayerManager::getInstance().groupSprinting(player, groupInfo->name, true);
                else if (sprinting == "gui.groupactionoperate.releasesprint"_tr())
                    res = SimPlayerManager::getInstance().groupSprinting(player, groupInfo->name, false);
                if (!res.second) {
                    flag = false;
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);
                }

                if (flag) {
                    pkt.mMessage.assign(res.first);
                    player->sendNetworkPacket(pkt);
                }
            }
        }
    );
}

void sendGroupMessageOperator(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.groupmessageoperate.title"_tr());
    menu.addDropdown(
        "operate",
        "gui.groupmessageoperate.operate"_tr(),
        std::vector<std::string>{"gui.groupmessageoperate.chat"_tr(), "gui.groupmessageoperate.runcmd"_tr()}
    );
    menu.addInput("text", "gui.groupmessageoperate.text"_tr());
    menu.sendTo(
        pl,
        [groupInfo](
            Player*                                                              player,
            std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
            lse::form::FormCancelReason                                          cancelReason
        ) {
            if (cancelReason.has_value()) sendGroupOperatorList(player, groupInfo);
            else {
                std::pair<std::string, bool> res;

                auto& op = elements["operate"]->value;
                if (op == "gui.groupmessageoperate.chat"_tr())
                    res = SimPlayerManager::getInstance().groupChat(player, groupInfo->name, elements["text"]->value);
                else if (op == "gui.groupmessageoperate.runcmd"_tr())
                    res = SimPlayerManager::getInstance().groupRunCmd(player, groupInfo->name, elements["text"]->value);
                TextPacket pkt = TextPacket();
                pkt.mType      = TextPacketType::Raw;
                if (res.second) pkt.mMessage.assign(res.first);
                else pkt.mMessage.assign("§c" + res.first + "§r");
                player->sendNetworkPacket(pkt);
            }
        }
    );
}

void sendGroupOperatorList(Player* pl, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo> groupInfo) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.groupoperatelist.title"_tr(), "");
    menu.addButton("gui.groupoperatelist.info"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupInfo(player, groupInfo);
    });
    if (pl->getUuid().asString() == groupInfo->owner)
        menu.addButton(
            "gui.groupoperatelist.manage"_tr(),
            "",
            [groupInfo](Player* player, lse::form::FormCancelReason) { sendGroupManageList(player, groupInfo); }
        );
    menu.addButton("gui.groupoperatelist.online"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().spawnGroup(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.groupoperatelist.offline"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().despawnGroup(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.groupoperatelist.respawn"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().respawnGroup(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.groupoperatelist.inv"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupInvOperatorList(player, groupInfo);
    });
    menu.addButton("gui.groupoperatelist.move"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupMoveOperator(player, groupInfo);
    });
    menu.addButton("gui.groupoperatelist.lookat"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupLookToOperator(player, groupInfo);
    });
    menu.addButton("gui.groupoperatelist.operate"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupOperator(player, groupInfo);
    });
    menu.addButton("gui.groupoperatelist.stop"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        auto       res = SimPlayerManager::getInstance().groupStop(player, groupInfo->name);
        TextPacket pkt = TextPacket();
        pkt.mType      = TextPacketType::Raw;
        if (res.second) pkt.mMessage.assign(res.first);
        else pkt.mMessage.assign("§c" + res.first + "§r");
        player->sendNetworkPacket(pkt);
    });
    menu.addButton("gui.groupoperatelist.action"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupActionOperator(player, groupInfo);
    });
    menu.addButton("gui.groupoperatelist.message"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
        sendGroupMessageOperator(player, groupInfo);
    });
    if (pl->getUuid().asString() == groupInfo->owner)
        menu.addButton("gui.groupoperatelist.rm"_tr(), "", [groupInfo](Player* player, lse::form::FormCancelReason) {
            auto       res = SimPlayerManager::getInstance().rmGroup(player, groupInfo->name);
            TextPacket pkt = TextPacket();
            pkt.mType      = TextPacketType::Raw;
            if (res.second) pkt.mMessage.assign(res.first);
            else pkt.mMessage.assign("§c" + res.first + "§r");
            player->sendNetworkPacket(pkt);
        });
    menu.sendTo(pl);
}

void sendCreateGroupPage(Player* pl, std::string defName = "") {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.creategroup.title"_tr());
    menu.addInput("name", "gui.creategroup.name"_tr(), "", defName);
    menu.sendTo(
        pl,
        [](Player*                                                              player,
           std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
           lse::form::FormCancelReason                                          cancelReason) {
            if (!cancelReason.has_value()) {
                auto       name = elements["name"]->value;
                auto       res  = SimPlayerManager::getInstance().createGroup(player, name);
                TextPacket pkt  = TextPacket();
                pkt.mType       = TextPacketType::Raw;
                if (res.second) {
                    pkt.mMessage.assign(res.first);
                    player->sendNetworkPacket(pkt);

                    auto tem = SimPlayerManager::getInstance().fetchGroup(name);
                    if (tem.has_value()) sendManageSpInGroup(player, tem.value());
                } else {
                    pkt.mMessage.assign("§c" + res.first + "§r");
                    player->sendNetworkPacket(pkt);

                    sendCreateGroupPage(player, name);
                }
            }
        }
    );
}

void sendAllGroupList(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::SimpleForm("gui.managergrouplist.title"_tr(), "");
    for (auto i : SimPlayerManager::getInstance().getAllGroupList()) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto groupInfo = SimPlayerManager::getInstance().fetchGroup(i);
            if (groupInfo.has_value()) sendGroupOperatorList(player, groupInfo.value());
        });
    }
    menu.addButton("gui.managergrouplist.newgroup"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendCreateGroupPage(player);
    });
    menu.sendTo(pl);
}

void sendManagePage(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto menu = lse::form::CustomForm("gui.manage.title"_tr());
    menu.addToggle("autojoin", "gui.manage.autojoin"_tr(), SimPlayerManager::getInstance().getAutoJoin());
    menu.addToggle("autorespawn", "gui.manage.autorespawn"_tr(), SimPlayerManager::getInstance().getAutoRespawn());
    menu.addToggle("autodespawn", "gui.manage.autodespawn"_tr(), SimPlayerManager::getInstance().getAutoDespawn());
    menu.sendTo(
        pl,
        [](Player*,
           std::map<std::string, std::shared_ptr<lse::form::CustomFormElement>> elements,
           lse::form::FormCancelReason                                          cancelReason) {
            if (!cancelReason.has_value()) {
                SimPlayerManager::getInstance().setAutoJoin(elements["autojoin"]->value == "1");
                SimPlayerManager::getInstance().setAutoRespawn(elements["autorespawn"]->value == "1");
                SimPlayerManager::getInstance().setAutoDespawn(elements["autodespawn"]->value == "1");
            }
        }
    );
}

void sendManagerMainMenu(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto mainMenu = lse::form::SimpleForm("gui.managermainmenu.title"_tr(), "");
    mainMenu.addButton("gui.managermainmenu.splistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendAllSplist(player);
    });
    mainMenu.addButton("gui.managermainmenu.grouplistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendAllGroupList(player);
    });
    mainMenu.addButton(
        "gui.managermainmenu.managerlistbutton"_tr(),
        "",
        [](Player* player, lse::form::FormCancelReason) { sendManagePage(player); }
    );
    mainMenu.sendTo(pl);
}

void sendMainMenu(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto mainMenu = lse::form::SimpleForm("gui.mainmenu.title"_tr(), "");
    mainMenu.addButton("gui.mainmenu.splistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendSplist(player);
    });
    mainMenu.addButton("gui.mainmenu.grouplistbutton"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
        sendGroupList(player);
    });
    mainMenu.sendTo(pl);
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
        menu.addButton("gui.splist.newsp"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
            sendNewSpPage(player);
        });
    menu.sendTo(pl);
}

void sendGroupList(Player* pl) {
    using ll::i18n_literals::operator""_tr;
    auto               menu  = lse::form::SimpleForm("gui.grouplist.title"_tr(), "");
    unsigned long long count = 0;
    for (auto i : SimPlayerManager::getInstance().fetchGroupList(pl)) {
        menu.addButton(i, "", [i](Player* player, lse::form::FormCancelReason) {
            auto groupInfo = SimPlayerManager::getInstance().fetchGroup(i);
            if (groupInfo.has_value()) sendGroupOperatorList(player, groupInfo.value());
        });
        count++;
    }
    if (pl->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
        || mod().getConfig().simPlayer.superManagerList.contains(*pl->mName)
        || count < mod().getConfig().simPlayer.maxGroup)
        menu.addButton("gui.grouplist.newgroup"_tr(), "", [](Player* player, lse::form::FormCancelReason) {
            sendCreateGroupPage(player);
        });
    menu.sendTo(pl);
}
} // namespace coral_fans::cfsp::gui