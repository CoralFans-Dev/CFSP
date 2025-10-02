#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/actor/player/LayeredAbilities.h"
#include "mc/world/level/Level.h"
#include "mc/world/phys/HitResult.h"
#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>


namespace coral_fans::cfsp::gui {

void GuiManager::sendOperateSpPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm(cfsp->mSaveData.name);
    uint perm;
    bool ismanager = manager::CFSPManager::getInstance().isManager(&player);
    if (ismanager) perm = manager::CFSPManager::getInstance().getSpPermissionMask();
    else
        perm = (uint)cfsp->getPermission(&player)
             & manager::CFSPManager::getInstance().getSpPermissionMask(
                 std::optional<CommandPermissionLevel>(player.getCommandPermissionLevel())
             );
    if (perm)
        form.appendButton("gui.operateSp.spinfo"_tr(), [this, cfsp](Player& player) {
            this->sendSpInfoPage(player, cfsp);
        });
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
            if (perm)
                form.appendButton("gui.operateSp.inv"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpInvOperatorPage(player, cfsp, perm);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpMessageOperatorPage(player, cfsp, perm);
                });
        } else {
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
                    || perm & (uint)simulated_player::SimPlayerPermission::Jump)
                    form.appendButton("gui.operateSp.perform"_tr(), [this, cfsp, perm](Player& player) {
                        this->sendSpActionOperatorPage(player, cfsp, perm);
                    });
                if (perm & (uint)simulated_player::SimPlayerPermission::Use
                    || perm & (uint)simulated_player::SimPlayerPermission::Destroy)
                    form.appendButton("gui.operateSp.perform2"_tr(), [this, cfsp, perm](Player& player) {
                        this->sendSpLongActionOperatorPage(player, cfsp, perm);
                    });
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
    if (perm & (uint)simulated_player::SimPlayerPermission::Delete)
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
                        return this->sendOperateSpPage(player, cfsp);
                    if (!result.has_value()) return;

                    if (result.value() == ll::form::ModalFormSelectedButton::Upper) {
                        if (!cfsp->mSimPlayer)
                            return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                        if (cfsp->mSaveData.isEmptyInv)
                            return manager::CFSPManager::getInstance()
                                .spDelete(&player, cfsp->mSaveData.name)
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
                                        .spDelete(&player, cfsp->mSaveData.name, true)
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
    if (cfsp->mSaveData.ownerUuid == player.getUuid().asString() || ismanager)
        form.appendButton("gui.operateSp.perm"_tr(), [this, cfsp](Player& player) {
            this->sendSpPermPage(player, cfsp);
        });

    form.sendTo(player);
}

void GuiManager::sendCreateSpPage(
    Player&     player,
    int         defDim,
    std::string defPos,
    std::string defName,
    int         defLockUniqueId
) {
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
    form.appendDropdown(
        "lockUniqueId",
        "gui.createSp.lockUniqueId"_tr(),
        std::vector<std::string>{"base.yesOrNo.yes"_tr(), "base.yesOrNo.no"_tr()},
        defLockUniqueId
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

            it = elements.value().find("lockUniqueId");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleLockUniqueId = std::get<std::string>(it->second);
            bool lockUniqueId;
            if (eleLockUniqueId == "base.yesOrNo.yes"_tr()) lockUniqueId = true;
            else if (eleLockUniqueId == "base.yesOrNo.no"_tr()) lockUniqueId = false;
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

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
                return this->sendCreateSpPage(player, dim, elePos, name, lockUniqueId);
            }
            manager::CFSPManager::getInstance()
                .spCreate(&player, name, targetPos.value(), dim, lockUniqueId)
                .sendTo(player);
        }
    );
}

void GuiManager::sendSpInfoPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    ll::form::SimpleForm(
        "gui.info.spTitle"_tr(),
        manager::CFSPManager::getInstance().spInfo(&player, cfsp->mSaveData.name).mInfo
    )
        .sendTo(player, [this, cfsp](Player& player, int, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                this->sendOperateSpPage(player, cfsp);
        });
}

void GuiManager::sendSpInvOperatorPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.inv.sp.title"_tr());
    if (perm)
        form.appendButton("gui.inv.sp.invinfo"_tr(), [this, cfsp, perm](Player& player) {
            ll::form::SimpleForm(
                "gui.inv.sp.invinfo"_tr(),
                manager::CFSPManager::getInstance().spInvInfo(&player, cfsp->mSaveData.name).mInfo
            )
                .sendTo(player, [this, cfsp, perm](Player& player, int, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        this->sendSpInvOperatorPage(player, cfsp, perm);
                });
        });
    if (perm & (uint)simulated_player::SimPlayerPermission::Swap)
        form.appendButton("gui.inv.sp.swap"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spSwap(&player, spname).sendTo(player);
        });
    if (perm & (uint)simulated_player::SimPlayerPermission::Drop)
        form.appendButton("gui.inv.sp.drop"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spDrop(&player, spname).sendTo(player);
        });
    if (perm & (uint)simulated_player::SimPlayerPermission::DropInv)
        form.appendButton("gui.inv.sp.dropinv"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spDropInv(&player, spname).sendTo(player);
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
    auto form = ll::form::CustomForm("gui.tp.spTitle"_tr());
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
                return this->sendSpTpOperatorPage(player, cfsp, dim, elePos);
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
    auto                     form = ll::form::CustomForm("gui.message.spTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Chat) op.emplace_back("gui.message.chat"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::RunCmd) op.emplace_back("gui.message.runcmd"_tr());
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

            if (eleOp == "gui.message.chat"_tr())
                return manager::CFSPManager::getInstance()
                    .spChat(&player, cfsp->mSaveData.name, eleText)
                    .sendTo(player);
            if (eleOp == "gui.message.runcmd"_tr())
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
    std::string                                  defSpeed,
    int                                          defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.move.spTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::MoveTo) op.emplace_back("gui.move.moveto"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::NavTo) op.emplace_back("gui.move.navto"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
    form.appendInput("speed", "gui.para.speed"_tr(), "4.3", defSpeed);
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

            it = elements.value().find("speed");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleSpeed = std::get<std::string>(it->second);
            auto speed    = this->tryGetFloat(eleSpeed);

            it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            if (!targetPos.has_value() || !speed.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendSpMoveOperatorPage(player, cfsp, perm, elePos, eleSpeed, opIndex);
            }
            if (eleOp == "gui.move.moveto"_tr())
                return manager::CFSPManager::getInstance()
                    .spMoveTo(&player, cfsp->mSaveData.name, targetPos.value(), speed.value())
                    .sendTo(player);
            if (eleOp == "gui.move.navto"_tr())
                return manager::CFSPManager::getInstance()
                    .spNavTo(&player, cfsp->mSaveData.name, targetPos.value(), speed.value())
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
    auto form = ll::form::CustomForm("gui.look.spTitle"_tr());
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
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
    auto                     form = ll::form::CustomForm("gui.action.spTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Attack) op.emplace_back("gui.action.attack"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Build) op.emplace_back("gui.action.build"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Interact) op.emplace_back("gui.action.interact"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Jump) op.emplace_back("gui.action.jump"_tr());
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
                return this->sendSpActionOperatorPage(player, cfsp, perm, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.action.attack"_tr())
                return manager::CFSPManager::getInstance()
                    .spAttack(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.action.build"_tr())
                return manager::CFSPManager::getInstance()
                    .spBuild(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.action.interact"_tr())
                return manager::CFSPManager::getInstance()
                    .spInteract(&player, cfsp->mSaveData.name, times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.action.jump"_tr())
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
    auto                     form = ll::form::CustomForm("gui.longAction.spTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)simulated_player::SimPlayerPermission::Use) op.emplace_back("gui.longAction.use"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Destroy) op.emplace_back("gui.longAction.destroy"_tr());
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
                return this->sendSpLongActionOperatorPage(player, cfsp, perm, eleLong, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.longAction.use"_tr())
                return manager::CFSPManager::getInstance()
                    .spUse(&player, cfsp->mSaveData.name, _long.value(), times.value(), interval.value())
                    .sendTo(player);
            if (eleOp == "gui.longAction.destroy"_tr())
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
    auto form = ll::form::CustomForm("gui.status.spTitle"_tr());
    if (perm & (uint)simulated_player::SimPlayerPermission::Sneaking)
        form.appendToggle(
            "sneaking",
            "gui.status.sneaking"_tr(),
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Sneaking)
        );
    if (perm & (uint)simulated_player::SimPlayerPermission::Swimming)
        form.appendToggle(
            "swimming",
            "gui.status.swimming"_tr(),
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Swimming)
        );
    if (perm & (uint)simulated_player::SimPlayerPermission::Flying
        && cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
        form.appendToggle("flying", "gui.status.flying"_tr(), cfsp->mSimPlayer->isFlying());
    if (perm & (uint)simulated_player::SimPlayerPermission::Sprinting)
        form.appendToggle(
            "sprinting",
            "gui.status.sprinting"_tr(),
            cfsp->mSimPlayer->getStatusFlag(ActorFlags::Sprinting)
        );
    form.sendTo(
        player,
        [this,
         cfsp](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateSpPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!cfsp->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);

            if (auto it = elements.value().find("sneaking");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    manager::CFSPManager::getInstance().spSneaking(&player, cfsp->mSaveData.name, true).sendTo(player);
                } else
                    manager::CFSPManager::getInstance().spSneaking(&player, cfsp->mSaveData.name, false).sendTo(player);
            }

            if (auto it = elements.value().find("swimming");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    manager::CFSPManager::getInstance().spSwimming(&player, cfsp->mSaveData.name, true).sendTo(player);
                } else
                    manager::CFSPManager::getInstance().spSwimming(&player, cfsp->mSaveData.name, false).sendTo(player);
            }

            if (cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
                if (auto it = elements.value().find("flying");
                    it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                    if ((bool)std::get<uint64>(it->second)) {
                        manager::CFSPManager::getInstance()
                            .spFlying(&player, cfsp->mSaveData.name, true)
                            .sendTo(player);
                    } else
                        manager::CFSPManager::getInstance()
                            .spFlying(&player, cfsp->mSaveData.name, false)
                            .sendTo(player);
                }

            if (auto it = elements.value().find("sprinting");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    manager::CFSPManager::getInstance().spSprinting(&player, cfsp->mSaveData.name, true).sendTo(player);
                } else
                    manager::CFSPManager::getInstance()
                        .spSprinting(&player, cfsp->mSaveData.name, false)
                        .sendTo(player);
            }
        }
    );
}

void GuiManager::sendSpPermPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    std::vector<std::pair<std::string, std::string>> splist;
    auto                                             level = ll::service::getLevel();
    if (level.has_value())
        level->forEachPlayer([&splist, pname = player.mName.get()](Player& player) {
            if (!player.isSimulatedPlayer() && pname != player.mName.get())
                splist.emplace_back(std::make_pair(player.mName, player.getUuid().asString()));
            return true;
        });
    for (auto i : cfsp->mSaveData.permission)
        splist.emplace_back(std::make_pair(base::utils::tryGetPlayerName(i.first), i.first));
    std::sort(splist.begin(), splist.end());

    auto form = ll::form::SimpleForm("gui.perm.spTitle"_tr());
    int  size = (int)splist.size();
    if (size) {
        form.appendButton(splist[0].first, [this, cfsp, targetPlayer = splist[0]](Player& player) {
            this->sendSpPermPage2(player, cfsp, targetPlayer.first, targetPlayer.second);
        });
        for (int i = 1; i < size; i++)
            if (splist[i] != splist[i - 1])
                form.appendButton(splist[i].first, [this, cfsp, targetPlayer = splist[i]](Player& player) {
                    this->sendSpPermPage2(player, cfsp, targetPlayer.first, targetPlayer.second);
                });
    }
    form.appendButton("gui.perm.publicSp"_tr(), [this, cfsp](Player& player) { this->sendSpPermPage2(player, cfsp); });
    form.sendTo(player, [this, cfsp](Player& player, int, ll::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
            this->sendOperateSpPage(player, cfsp);
    });
}

void GuiManager::sendSpPermPage2(
    Player&                                      player,
    std::shared_ptr<simulated_player::SimPlayer> cfsp,
    std::string                                  targetPlayerName,
    std::string                                  targetPlayerUuid
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.perm.spTitle2"_tr(
        cfsp->mSaveData.name,
        targetPlayerName != "" ? targetPlayerName : "gui.perm.publicSp"_tr()
    ));

    uint perm = 0;
    if (targetPlayerName == "") perm = cfsp->mSaveData.publicPermission;
    else {
        auto it = cfsp->mSaveData.permission.find(targetPlayerUuid);
        if (it != cfsp->mSaveData.permission.end()) perm = it->second;
    }

    form.appendToggle("spawn", "gui.perm.spawn"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Spawn);
    form.appendToggle("despawn", "gui.perm.despawn"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Despawn);
    form.appendToggle("respawn", "gui.perm.respawn"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Respawn);
    form.appendToggle("delete", "gui.perm.delete"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Delete);
    form.appendToggle("stop", "gui.perm.stop"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Stop);
    form.appendToggle("drop", "gui.perm.drop"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Drop);
    form.appendToggle("dropInv", "gui.perm.dropInv"_tr(), perm & (uint)simulated_player::SimPlayerPermission::DropInv);
    form.appendToggle("swap", "gui.perm.swap"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Swap);
    form.appendToggle(
        "sneaking",
        "gui.perm.sneaking"_tr(),
        perm & (uint)simulated_player::SimPlayerPermission::Sneaking
    );
    form.appendToggle(
        "swimming",
        "gui.perm.swimming"_tr(),
        perm & (uint)simulated_player::SimPlayerPermission::Swimming
    );
    form.appendToggle("flying", "gui.perm.flying"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Flying);
    form.appendToggle(
        "sprinting",
        "gui.perm.sprinting"_tr(),
        perm & (uint)simulated_player::SimPlayerPermission::Sprinting
    );
    form.appendToggle("attack", "gui.perm.attack"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Attack);
    form.appendToggle("build", "gui.perm.build"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Build);
    form.appendToggle(
        "interact",
        "gui.perm.interact"_tr(),
        perm & (uint)simulated_player::SimPlayerPermission::Interact
    );
    form.appendToggle("jump", "gui.perm.jump"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Jump);
    form.appendToggle("use", "gui.perm.use"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Use);
    form.appendToggle("destroy", "gui.perm.destroy"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Destroy);
    form.appendToggle("chat", "gui.perm.chat"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Chat);
    form.appendToggle("runCmd", "gui.perm.runCmd"_tr(), perm & (uint)simulated_player::SimPlayerPermission::RunCmd);
    form.appendToggle("select", "gui.perm.select"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Select);
    form.appendToggle("lookAt", "gui.perm.lookAt"_tr(), perm & (uint)simulated_player::SimPlayerPermission::LookAt);
    form.appendToggle("moveTo", "gui.perm.moveTo"_tr(), perm & (uint)simulated_player::SimPlayerPermission::MoveTo);
    form.appendToggle("navTo", "gui.perm.navTo"_tr(), perm & (uint)simulated_player::SimPlayerPermission::NavTo);
    form.appendToggle("tp", "gui.perm.tp"_tr(), perm & (uint)simulated_player::SimPlayerPermission::Tp);
    form.appendToggle(
        "beAddedToGroup",
        "gui.perm.beAddedToGroup"_tr(),
        perm & (uint)simulated_player::SimPlayerPermission::BeAddedToGroup
    );

    form.sendTo(
        player,
        [this, cfsp, targetPlayerUuid](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendSpPermPage(player, cfsp);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            uint newPerm = 0;

            if (auto it = elements.value().find("spawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Spawn;

            if (auto it = elements.value().find("despawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Despawn;

            if (auto it = elements.value().find("respawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Respawn;

            if (auto it = elements.value().find("delete");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Delete;

            if (auto it = elements.value().find("stop");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Stop;

            if (auto it = elements.value().find("drop");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Drop;

            if (auto it = elements.value().find("dropInv");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::DropInv;

            if (auto it = elements.value().find("swap");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Swap;

            if (auto it = elements.value().find("sneaking");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second))
                    newPerm |= (uint)simulated_player::SimPlayerPermission::Sneaking;

            if (auto it = elements.value().find("swimming");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second))
                    newPerm |= (uint)simulated_player::SimPlayerPermission::Swimming;

            if (auto it = elements.value().find("flying");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Flying;

            if (auto it = elements.value().find("sprinting");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second))
                    newPerm |= (uint)simulated_player::SimPlayerPermission::Sprinting;

            if (auto it = elements.value().find("attack");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Attack;

            if (auto it = elements.value().find("build");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Build;

            if (auto it = elements.value().find("interact");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second))
                    newPerm |= (uint)simulated_player::SimPlayerPermission::Interact;

            if (auto it = elements.value().find("jump");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Jump;

            if (auto it = elements.value().find("use");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Use;

            if (auto it = elements.value().find("destroy");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Destroy;

            if (auto it = elements.value().find("chat");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Chat;

            if (auto it = elements.value().find("runCmd");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::RunCmd;

            if (auto it = elements.value().find("select");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Select;

            if (auto it = elements.value().find("lookAt");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::LookAt;

            if (auto it = elements.value().find("moveTo");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::MoveTo;

            if (auto it = elements.value().find("navTo");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::NavTo;

            if (auto it = elements.value().find("tp");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)simulated_player::SimPlayerPermission::Tp;

            if (auto it = elements.value().find("beAddedToGroup");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second))
                    newPerm |= (uint)simulated_player::SimPlayerPermission::BeAddedToGroup;


            if (targetPlayerUuid == "") cfsp->mSaveData.publicPermission = newPerm;
            else if (newPerm) cfsp->mSaveData.permission[targetPlayerUuid] = newPerm;
            else if (auto it = cfsp->mSaveData.permission.find(targetPlayerUuid);
                     it != cfsp->mSaveData.permission.end())
                cfsp->mSaveData.permission.erase(it);

            cfsp->mShouldSave = true;

            base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
        }
    );
}
} // namespace coral_fans::cfsp::gui