#include "SpMovePage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void SpMovePage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.move.spTitle"_tr());
    std::vector<std::string> op;
    if (mPerm & (uint)simulated_player::SimPlayerPermission::MoveTo) op.emplace_back("gui.move.moveto"_tr());
    if (mPerm & (uint)simulated_player::SimPlayerPermission::NavTo) op.emplace_back("gui.move.navto"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, mDefOp);
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", mDefPos);
    form.appendInput("speed", "gui.para.speed"_tr(), "4.3", mDefSpeed);
    form.sendTo(
        player,
        [this,
         self = shared_from_this(),
         op](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto elePos = utils::getStringElement(elements, "targetpos");
            if (!elePos.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto targetPos = utils::tryGetVec3(elePos.value());

            auto eleSpeed = utils::getStringElement(elements, "speed");
            if (!eleSpeed.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto speed = utils::tryGetFloat(eleSpeed.value());

            auto eleOp = utils::getStringElement(elements, "operate");
            if (!eleOp.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value() || !speed.has_value()) {
                int opIndex = utils::findOpIndex(op, eleOp.value());
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                mDefPos   = elePos.value();
                mDefSpeed = eleSpeed.value();
                mDefOp    = opIndex;
                return this->sendTo(player);
            }
            if (eleOp.value() == "gui.move.moveto"_tr())
                return manager::CFSPManager::getInstance()
                    .spMoveTo(&player, mCfsp->mSaveData.name, targetPos.value(), speed.value())
                    .sendTo(player);
            if (eleOp.value() == "gui.move.navto"_tr())
                return manager::CFSPManager::getInstance()
                    .spNavTo(&player, mCfsp->mSaveData.name, targetPos.value(), speed.value())
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
