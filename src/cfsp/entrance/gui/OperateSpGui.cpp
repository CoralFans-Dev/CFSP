#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/actor/player/LayeredAbilities.h"
#include "mc/world/phys/HitResult.h"
#include <optional>
#include <string>
#include <vector>

namespace coral_fans::cfsp::gui {

void GuiManager::sendOperateSpPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm(cfsp->mSaveData.name);
    form.appendButton("gui.operateSp.spinfo"_tr(), [this, cfsp](Player& player) {
        this->sendSpInfoPage(player, cfsp);
    });
    uint perm;
    if (manager::CFSPManager::getInstance().isManager(&player))
        perm = manager::CFSPManager::getInstance().getSpPermissionMask();
    else
        perm = (uint)cfsp->getPermission(&player)
             & manager::CFSPManager::getInstance().getSpPermissionMask(
                 std::optional<CommandPermissionLevel>(player.getCommandPermissionLevel())
             );
    if (!cfsp->mSimPlayer) {
        if (perm & (uint)simulated_player::SimPlayerPermission::Spawn)
            form.appendButton("gui.operateSp.online"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spSpawn(&player, spname).sendTo(player);
            });
    } else {
        if (perm & (uint)simulated_player::SimPlayerPermission::Despawn)
            form.appendButton("gui.operateSp.offline"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spDespawn(&player, spname).sendTo(player);
            });
        if (cfsp->mSimPlayer->isDead()) {
            if (perm & (uint)simulated_player::SimPlayerPermission::Respawn)
                form.appendButton("gui.operateSp.respawn"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spRespawn(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Tp)
                form.appendButton("gui.operateSp.tp"_tr(), [this, cfsp](Player& player) {
                    Vec3        pos;
                    const auto& hit = player.traceRay(5.25f, false, true);
                    if (hit) pos = hit.mPos;
                    else pos = player.getFeetPos();
                    this->sendSpTpOperatorPage(
                        player,
                        cfsp,
                        player.getDimensionId(),
                        std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
                    );
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Swap)
                form.appendButton("gui.operateSp.swap"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spSwap(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpMessageOperatorPage(player, cfsp, perm);
                });
        } else {
            if (perm & (uint)simulated_player::SimPlayerPermission::Swap
                || perm & (uint)simulated_player::SimPlayerPermission::Drop
                || perm & (uint)simulated_player::SimPlayerPermission::DropInv)
                form.appendButton("gui.operateSp.inv"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpInvOperatorPage(player, cfsp, perm);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Tp)
                form.appendButton("gui.operateSp.tp"_tr(), [this, cfsp](Player& player) {
                    Vec3        pos;
                    const auto& hit = player.traceRay(5.25f, false, true);
                    if (hit) pos = hit.mPos;
                    else pos = player.getFeetPos();
                    this->sendSpTpOperatorPage(
                        player,
                        cfsp,
                        player.getDimensionId(),
                        std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
                    );
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::LookAt)
                form.appendButton("gui.operateSp.lookat"_tr(), [this, cfsp](Player& player) {
                    if (!cfsp->mSimPlayer)
                        return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                    if (player.getDimensionId() != cfsp->mSimPlayer->getDimensionId())
                        this->sendSpLookOperatorPage(player, cfsp);
                    else {
                        Vec3        pos;
                        const auto& hit = player.traceRay(5.25f, false, true);
                        if (hit) pos = hit.mPos;
                        else pos = player.getFeetPos();
                        this->sendSpLookOperatorPage(
                            player,
                            cfsp,
                            std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
                        );
                    }
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::MoveTo
                || perm & (uint)simulated_player::SimPlayerPermission::NavTo)
                form.appendButton("gui.operateSp.move"_tr(), [this, cfsp, perm](Player& player) {
                    if (!cfsp->mSimPlayer)
                        return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                    if (player.getDimensionId() != cfsp->mSimPlayer->getDimensionId())
                        this->sendSpMoveOperatorPage(player, cfsp, perm);
                    else {
                        Vec3        pos;
                        const auto& hit = player.traceRay(5.25f, false, true);
                        if (hit) pos = hit.mPos;
                        else pos = player.getFeetPos();
                        this->sendSpMoveOperatorPage(
                            player,
                            cfsp,
                            perm,
                            std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
                        );
                    }
                });
            if (cfsp->isFree()) {
                if (perm & (uint)simulated_player::SimPlayerPermission::Attack
                    || perm & (uint)simulated_player::SimPlayerPermission::Build
                    || perm & (uint)simulated_player::SimPlayerPermission::Interact
                    || perm & (uint)simulated_player::SimPlayerPermission::Jump) {
                    form.appendButton("gui.operateSp.perform"_tr(), [this, cfsp, perm](Player& player) {
                        this->sendSpActionOperatorPage(player, cfsp, perm);
                    });
                    form.appendButton("gui.operateSp.perform2"_tr(), [this, cfsp, perm](Player& player) {
                        this->sendSpLongActionOperatorPage(player, cfsp, perm);
                    });
                }
            }
            if (perm & (uint)simulated_player::SimPlayerPermission::Stop)
                form.appendButton("gui.operateSp.stop"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spStop(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Sneaking
                || perm & (uint)simulated_player::SimPlayerPermission::Swimming
                || (perm & (uint)simulated_player::SimPlayerPermission::Flying
                    && cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
                || perm & (uint)simulated_player::SimPlayerPermission::Sprinting)
                form.appendButton("gui.operateSp.status"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpStatusOperatorPage(player, cfsp, perm);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpMessageOperatorPage(player, cfsp, perm);
                });
        }
    }
    if (perm & (uint)simulated_player::SimPlayerPermission::Rm)
        form.appendButton("gui.operateSp.rm"_tr(), [this, cfsp](Player& player) {
            auto confirmForm = ll::form::ModalForm(
                "gui.operateSp.rm"_tr(),
                "gui.operateSp.confirmRm"_tr(cfsp->mSaveData.name),
                "base.yesOrNo.yes"_tr(),
                "base.yesOrNo.no"_tr()
            );
            confirmForm.sendTo(
                player,
                [this,
                 cfsp](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        this->sendOperateSpPage(player, cfsp);
                    if (!result.has_value()) return;

                    if (result.value() == ll::form::ModalFormSelectedButton::Upper) {
                        if (!cfsp->mSimPlayer)
                            return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                        if (cfsp->mSaveData.isEmptyInv)
                            return manager::CFSPManager::getInstance()
                                .spRm(&player, cfsp->mSaveData.name)
                                .sendTo(player);
                        auto confirmForm2 = ll::form::ModalForm(
                            "gui.operateSp.rm"_tr(),
                            "gui.operateSp.confirmRm2"_tr(cfsp->mSaveData.name),
                            "base.yesOrNo.yes"_tr(),
                            "base.yesOrNo.no"_tr()
                        );
                        confirmForm2.sendTo(
                            player,
                            [this, cfsp](
                                Player&                    player,
                                ll::form::ModalFormResult  result,
                                ll::form::FormCancelReason cancelReason
                            ) {
                                if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                                    this->sendOperateSpPage(player, cfsp);
                                if (!result.has_value()) return;

                                if (result.value() == ll::form::ModalFormSelectedButton::Upper)
                                    return manager::CFSPManager::getInstance()
                                        .spRm(&player, cfsp->mSaveData.name)
                                        .sendTo(player);
                                if (result.value() == ll::form::ModalFormSelectedButton::Lower)
                                    return this->sendOperateSpPage(player, cfsp);
                            }
                        );
                    } else if (result.value() == ll::form::ModalFormSelectedButton::Lower)
                        this->sendOperateSpPage(player, cfsp);
                }
            );
        });
    form.sendTo(player);
}

void GuiManager::sendCreateSpPage(Player& player, int defDim, std::string defPos, std::string defName) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.createSp.title"_tr());
    form.appendInput("name", "gui.createSp.name"_tr(), "", defName);
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
    form.appendDropdown(
        "dim",
        "gui.para.targetDim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
    form.sendTo(
        player,
        [this](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("name");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second)) {
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            }
            auto name = std::get<std::string>(it->second);

            it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("dim");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleDim = std::get<std::string>(it->second);
            int  dim;
            if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
            else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
            else if (eleDim == "base.dimension.theend"_tr()) dim = 2;
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendCreateSpPage(player, dim, elePos, name);
            }
            auto res = manager::CFSPManager::getInstance().spCreate(&player, name, targetPos.value(), dim);
            res.sendTo(player);
            if (!res) this->sendCreateSpPage(player, dim, elePos, name);
        }
    );
}

void GuiManager::sendSpInfoPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    ll::form::SimpleForm(
        "gui.spinfo.title"_tr(),
        manager::CFSPManager::getInstance().spInfo(&player, cfsp->mSaveData.name).mInfo
    )
        .sendTo(player, [this, cfsp](Player& player, int, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                this->sendOperateSpPage(player, cfsp);
        });
}

void GuiManager::sendSpInvOperatorPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.spinv.title"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Swap)
        form.appendButton("gui.spinv.swap"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spSwap(&player, spname);
        });
    if (perm & (uint)simulated_player::SimPlayerPermission::Drop)
        form.appendButton("gui.spinv.drop"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spDrop(&player, spname);
        });
    if (perm & (uint)simulated_player::SimPlayerPermission::DropInv)
        form.appendButton("gui.spinv.dropinv"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spDropInv(&player, spname);
        });
    form.sendTo(player, [this, cfsp](Player& player, int, ll::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
            this->sendOperateSpPage(player, cfsp);
    });
}

void GuiManager::sendSpTpOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    int                                          defDim,
    std::string                                  defPos
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.spTp.title"_tr());
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
    form.appendDropdown(
        "dim",
        "gui.para.targetDim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
    form.sendTo(
        player,
        [this,
         cfsp](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("dim");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleDim = std::get<std::string>(it->second);
            int  dim;
            if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
            else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
            else if (eleDim == "base.dimension.theend"_tr()) dim = 2;
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                this->sendSpTpOperatorPage(player, cfsp, dim, elePos);
            }
            manager::CFSPManager::getInstance()
                .spTp(&player, cfsp->mSaveData.name, targetPos.value(), dim)
                .sendTo(player);
        }
    );
}

void GuiManager::sendSpMessageOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    uint                                         perm
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.spMessage.title"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Chat) op.emplace_back("gui.spMessage.chat"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::RunCmd) op.emplace_back("gui.spMessage.runcmd"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op);
    form.appendInput("text", "gui.para.text"_tr());
    form.sendTo(
        player,
        [this,
         cfsp](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("text");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleText = std::get<std::string>(it->second);

            if (eleOp == "gui.spMessage.chat"_tr())
                return manager::CFSPManager::getInstance()
                    .spChat(&player, cfsp->mSaveData.name, eleText)
                    .sendTo(player);
            if (eleOp == "gui.spMessage.runcmd"_tr())
                return manager::CFSPManager::getInstance()
                    .spRunCmd(&player, cfsp->mSaveData.name, eleText)
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendSpMoveOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    uint                                         perm,
    std::string                                  defPos,
    int                                          defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.spMessage.title"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::MoveTo) op.emplace_back("gui.spMove.moveto"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::NavTo) op.emplace_back("gui.spMove.navto"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
    form.sendTo(
        player,
        [this,
         cfsp,
         perm,
         op](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            if (!targetPos.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                this->sendSpMoveOperatorPage(player, cfsp, perm, elePos, opIndex);
            }

            if (eleOp == "gui.spMove.moveto"_tr())
                return manager::CFSPManager::getInstance()
                    .spMoveTo(&player, cfsp->mSaveData.name, targetPos.value())
                    .sendTo(player);
            if (eleOp == "gui.spMove.navto"_tr())
                return manager::CFSPManager::getInstance()
                    .spNavTo(&player, cfsp->mSaveData.name, targetPos.value())
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendSpLookOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    std::string                                  defPos
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.spLook.title"_tr());
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
    form.sendTo(
        player,
        [this,
         cfsp](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendSpLookOperatorPage(player, cfsp, elePos);
            }

            return manager::CFSPManager::getInstance()
                .spLookAt(&player, cfsp->mSaveData.name, targetPos.value())
                .sendTo(player);
        }
    );
}

void GuiManager::sendSpActionOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    uint                                         perm,
    std::string                                  defTimes,
    std::string                                  defInterval,
    int                                          defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.spAction.title"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Attack) op.emplace_back("gui.spAction.attack"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Build) op.emplace_back("gui.spAction.build"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Interact) op.emplace_back("gui.spAction.interact"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Jump) op.emplace_back("gui.spAction.jump"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         cfsp,
         op,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!times.has_value() || !interval.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                this->sendSpActionOperatorPage(player, cfsp, perm, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.spAction.attack"_tr())
                return manager::CFSPManager::getInstance()
                    .spAttack(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.spAction.build"_tr())
                return manager::CFSPManager::getInstance()
                    .spBuild(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.spAction.interact"_tr())
                return manager::CFSPManager::getInstance()
                    .spInteract(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.spAction.jump"_tr())
                return manager::CFSPManager::getInstance()
                    .spJump(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendSpLongActionOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    uint                                         perm,
    std::string                                  defLong,
    std::string                                  defTimes,
    std::string                                  defInterval,
    int                                          defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.spLongAction.title"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Use) op.emplace_back("gui.spLongAction.use"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Destroy) op.emplace_back("gui.spLongAction.destroy"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("long", "gui.para.long"_tr(), "10", defLong);
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         cfsp,
         op,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("long");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleLong = std::get<std::string>(it->second);
            auto _long   = this->tryGetInt(eleLong);

            it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!_long.has_value() || !times.has_value() || !interval.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                this->sendSpLongActionOperatorPage(player, cfsp, perm, eleLong, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.spLongAction.use"_tr())
                return manager::CFSPManager::getInstance()
                    .spUse(&player, cfsp->mSaveData.name, _long.value(), times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.spLongAction.destroy"_tr())
                return manager::CFSPManager::getInstance()
                    .spDestroy(&player, cfsp->mSaveData.name, _long.value(), times.value(), interval.value())
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendSpStatusOperatorPage(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    uint                                         perm
) {
    using ll::i18n_literals::operator""_tr;
    if (!cfsp->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
    auto form = ll::form::CustomForm("gui.spStatus.title"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Sneaking)
        form.appendDropdown(
            "sneaking",
            "gui.spStatus.sneaking"_tr(),
            std::vector<std::string>{"gui.spStatus.sneak"_tr(), "gui.spStatus.releasesneak"_tr()},
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Sneaking) ? 0 : 1
        );
    if (perm & (uint)simulated_player::SimPlayerPermission::Swimming)
        form.appendDropdown(
            "swimming",
            "gui.spStatus.swimming"_tr(),
            std::vector<std::string>{"gui.spStatus.swim"_tr(), "gui.spStatus.releaseswim"_tr()},
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Swimming) ? 0 : 1
        );
    if (perm & (uint)simulated_player::SimPlayerPermission::Flying
        && cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
        form.appendDropdown(
            "flying",
            "gui.spStatus.flying"_tr(),
            std::vector<std::string>{"gui.spStatus.fly"_tr(), "gui.spStatus.releasefly"_tr()},
            cfsp->mSimPlayer->isFlying() ? 0 : 1
        );
    if (perm & (uint)simulated_player::SimPlayerPermission::Sprinting)
        form.appendDropdown(
            "sprinting",
            "gui.spStatus.sprinting"_tr(),
            std::vector<std::string>{"gui.spStatus.sprint"_tr(), "gui.spStatus.releasesprint"_tr()},
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Sprinting) ? 0 : 1
        );
    form.sendTo(
        player,
        [this,
         cfsp](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!cfsp->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);

            auto it = elements.value().find("sneaking");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto sneaking = std::get<std::string>(it->second);

            it = elements.value().find("swimming");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto swimming = std::get<std::string>(it->second);

            std::string flying;
            if (cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal) {
                it = elements.value().find("flying");
                if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                    return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                flying = std::get<std::string>(it->second);
            }

            it = elements.value().find("sprinting");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto sprinting = std::get<std::string>(it->second);

            if (sneaking == "gui.spStatus.sneak"_tr())
                manager::CFSPManager::getInstance().spSneaking(&player, cfsp->mSaveData.name, true).sendTo(player);
            else if (sneaking == "gui.spStatus.releasesneak"_tr())
                manager::CFSPManager::getInstance().spSneaking(&player, cfsp->mSaveData.name, false).sendTo(player);

            if (swimming == "gui.spStatus.swim"_tr())
                manager::CFSPManager::getInstance().spSwimming(&player, cfsp->mSaveData.name, true).sendTo(player);
            else if (swimming == "gui.spStatus.releaseswim"_tr())
                manager::CFSPManager::getInstance().spSwimming(&player, cfsp->mSaveData.name, false).sendTo(player);

            if (cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal) {
                if (flying == "gui.spStatus.fly"_tr())
                    manager::CFSPManager::getInstance().spFlying(&player, cfsp->mSaveData.name, true).sendTo(player);
                else if (flying == "gui.spStatus.releasefly"_tr())
                    manager::CFSPManager::getInstance().spFlying(&player, cfsp->mSaveData.name, false).sendTo(player);
            }

            if (sprinting == "gui.spStatus.sprint"_tr())
                manager::CFSPManager::getInstance().spSprinting(&player, cfsp->mSaveData.name, true).sendTo(player);
            else if (sprinting == "gui.spStatus.releasesprint"_tr())
                manager::CFSPManager::getInstance().spSprinting(&player, cfsp->mSaveData.name, false).sendTo(player);
        }
    );
}
} // namespace coral_fans::cfsp::gui