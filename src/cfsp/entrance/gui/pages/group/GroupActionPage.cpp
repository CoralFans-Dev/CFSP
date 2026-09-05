#include "GroupActionPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void GroupActionPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.action.groupTitle"_tr());
    std::vector<std::string> op;
    if (mPerm & (uint)group::GroupPermission::Attack) op.emplace_back("gui.action.attack"_tr());
    if (mPerm & (uint)group::GroupPermission::Build) op.emplace_back("gui.action.build"_tr());
    if (mPerm & (uint)group::GroupPermission::Interact) op.emplace_back("gui.action.interact"_tr());
    if (mPerm & (uint)group::GroupPermission::Jump) op.emplace_back("gui.action.jump"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, mDefOp);
    form.appendInput("times", "gui.para.times"_tr(), "1", mDefTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", mDefInterval);
    form.sendTo(
        player,
        [this,
         self = shared_from_this(),
         op](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto eleOp = utils::getStringElement(elements, "operate");
            if (!eleOp.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto eleTimes = utils::getStringElement(elements, "times");
            if (!eleTimes.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto times = utils::tryGetInt(eleTimes.value());

            auto eleInterval = utils::getStringElement(elements, "interval");
            if (!eleInterval.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto interval = utils::tryGetInt(eleInterval.value());

            if (!times.has_value() || !interval.has_value()) {
                int opIndex = utils::findOpIndex(op, eleOp.value());
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                mDefTimes    = eleTimes.value();
                mDefInterval = eleInterval.value();
                mDefOp       = opIndex;
                return this->sendTo(player);
            }

            if (eleOp.value() == "gui.action.attack"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupAttack(&player, mGroup->mData.name, times.value(), interval.value())
                );
            if (eleOp.value() == "gui.action.build"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupBuild(&player, mGroup->mData.name, times.value(), interval.value())
                );
            if (eleOp.value() == "gui.action.interact"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupInteract(&player, mGroup->mData.name, times.value(), interval.value())
                );
            if (eleOp.value() == "gui.action.jump"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupJump(&player, mGroup->mData.name, times.value(), interval.value())
                );
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
