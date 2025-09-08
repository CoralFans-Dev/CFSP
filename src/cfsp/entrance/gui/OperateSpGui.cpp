#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/actor/player/LayeredAbilities.h"
#include "mc/world/actor/provider/ActorAttribute.h"
#include <optional>

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
                    this->sendSpTpOperatorPage(player, cfsp);
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
                    this->sendSpTpOperatorPage(player, cfsp);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::LookAt)
                form.appendButton("gui.operateSp.lookat", [this, cfsp](Player& player) {
                    this->sendSpLookOperatorPage(player, cfsp);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::MoveTo
                || perm & (uint)simulated_player::SimPlayerPermission::NavTo)
                form.appendButton("gui.operateSp.move"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpMoveOperatorPage(player, cfsp, perm);
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
            } else if (perm & (uint)simulated_player::SimPlayerPermission::Stop)
                form.appendButton("gui.operateSp.stop"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spStop(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Sneaking
                || perm & (uint)simulated_player::SimPlayerPermission::Swimming
                || (perm & (uint)simulated_player::SimPlayerPermission::Flying
                    && cfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
                || perm & (uint)simulated_player::SimPlayerPermission::Sprinting)
                form.appendButton("gui.operateSp.lookat", [this, cfsp, perm](Player& player) {
                    this->sendSpStatusOperatorPage(player, cfsp, perm);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, cfsp, perm](Player& player) {
                    this->sendSpMessageOperatorPage(player, cfsp, perm);
                });
        }
        if (perm & (uint)simulated_player::SimPlayerPermission::Rm)
            form.appendButton("gui.operateSp.rm"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spRm(&player, spname).sendTo(player);
            });
    }
}

void GuiManager::sendNewSpPage(Player& player, int defDim, std::string defPos, std::string defName) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.createSp.title"_tr());
    form.appendInput("name", "gui.createSp.name"_tr(), "", defName);
    form.appendInput("targetpos", "gui.createSp.targetPos"_tr(), "0 0 0", defPos);
    form.appendDropdown(
        "dim",
        "gui.createSp.targetDim"_tr(),
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
            if (!elements.has_value()) return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("name");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second)) {
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            }
            std::string name = std::get<std::string>(it->second);

            it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("dim");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            auto eleDim = std::get<std::string>(it->second);
            int  dim;
            if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
            else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
            else if (eleDim == "base.dimension.theend"_tr()) dim = 2;
            else return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.createSp.nameError"_tr()).sendTo(player);
                return this->sendNewSpPage(player, dim, elePos, name);
            }
            auto res = manager::CFSPManager::getInstance().spCreate(&player, name, targetPos.value(), dim);
            res.sendTo(player);
            if (!res) this->sendNewSpPage(player, dim, elePos, name);
        }
    );
}

void GuiManager::sendSpInfoPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    std::string content  = "\n  " + "gui.spinfo.name"_tr() + cfsp->mSaveData.name + "\n  ";
    content             += "gui.spinfo.owner"_tr() + base::utils::tryGetPlayerName(cfsp->mSaveData.ownerUuid) + "\n  ";
    content             += "gui.spinfo.status"_tr()
             + (cfsp->mSimPlayer              ? "base.spstatus.offline"_tr()
                : cfsp->mSimPlayer->isAlive() ? "base.spstatus.alive"_tr()
                                              : "base.spstatus.dead"_tr());
    if (cfsp->mSimPlayer) {
        content += "gui.spinfo.pos"_tr() + base::utils::getDimName(cfsp->mSimPlayer->getDimensionId()) + " "
                 + cfsp->mSimPlayer->getPosition().toJsonString() + "\n  ";
        content += "gui.spinfo.respawnpos"_tr()
                 + base::utils::getDimName(cfsp->mSimPlayer->mPlayerRespawnPoint->mDimension->id) + " "
                 + cfsp->mSimPlayer->mPlayerRespawnPoint->mPlayerPosition->toString() + "\n  ";
        content += "gui.spinfo.gamemode"_tr() + base::utils::getGameModeStr((int)cfsp->mSimPlayer->getPlayerGameType())
                 + "\n  ";
        content += "gui.spinfo.health"_tr()
                 + std::to_string(ActorAttribute::getHealth(cfsp->mSimPlayer->getEntityContext())) + " / "
                 + std::to_string(cfsp->mSimPlayer->getMaxHealth()) + "\n  ";
        content += "gui.spinfo.isFree"_tr() + (cfsp->isFree() ? "base.yesno.yes"_tr() : "base.yesno.no"_tr());
    }
    ll::form::SimpleForm("gui.spinfo.title"_tr(), content)
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
}
} // namespace coral_fans::cfsp::gui