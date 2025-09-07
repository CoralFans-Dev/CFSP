#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"


namespace coral_fans::cfsp::gui {
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

void GuiManager::sendOperateSpPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm(cfsp->mSaveData.name);
    form.appendButton("gui.oprateSp.spinfo"_tr(), [this, cfsp](Player& player) { this->sendSpInfo(player, cfsp); });
    simulated_player::SimPlayerPermission perm;
    if (manager::CFSPManager::getInstance().isManager(&player)) perm = (simulated_player::SimPlayerPermission)-1;
    else perm = cfsp->getPermission(&player);
    if (!cfsp->mSimPlayer) {
        if ((uint)perm & (uint)simulated_player::SimPlayerPermission::Spawn)
            form.appendButton("gui.oprateSp.online"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spSpawn(&player, spname).sendTo(player);
            });
    } else {
        if ((uint)perm & (uint)simulated_player::SimPlayerPermission::Despawn)
            form.appendButton("gui.oprateSp.offline"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spDespawn(&player, spname).sendTo(player);
            });
        if (cfsp->mSimPlayer->isDead()) {
            if ((uint)perm & (uint)simulated_player::SimPlayerPermission::Respawn)
                form.appendButton("gui.oprateSp.respawn"_tr(), [spname = cfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spRespawn(&player, spname).sendTo(player);
                });
            if ((uint)perm & (uint)simulated_player::SimPlayerPermission::Tp)
                form.appendButton("gui.oprateSp.tp"_tr(), [spname = cfsp->mSaveData.name](Player& player) {

                });
        } else {
            if ((uint)perm & (uint)simulated_player::SimPlayerPermission::Swap
                || (uint)perm & (uint)simulated_player::SimPlayerPermission::Drop
                || (uint)perm & (uint)simulated_player::SimPlayerPermission::DropInv
                || (uint)perm & (uint)simulated_player::SimPlayerPermission::Select)
                form.appendButton("gui.oprateSp.inv"_tr(), [this, cfsp](Player& player) {
                    this->sendSpInvOperatorPage(player, cfsp);
                });
            if ((uint)perm
                & ((uint)simulated_player::SimPlayerPermission::MoveTo
                   | (uint)simulated_player::SimPlayerPermission::NavTo
                   | (uint)simulated_player::SimPlayerPermission::Tp))
                form.appendButton("gui.oprateSp.inv"_tr(), [this, cfsp](Player& player) {
                    this->sendSpInvOperatorPage(player, cfsp);
                });
        }
    }
}
} // namespace coral_fans::cfsp::gui