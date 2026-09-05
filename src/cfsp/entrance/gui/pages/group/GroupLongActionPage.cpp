#include "GroupLongActionPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void GroupLongActionPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.longAction.groupTitle"_tr());
    std::vector<std::string> op;
    if (mPerm & (uint)group::GroupPermission::Use) op.emplace_back("gui.longAction.use"_tr());
    if (mPerm & (uint)group::GroupPermission::Destroy) op.emplace_back("gui.longAction.destroy"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, mDefOp);
    form.appendInput("long", "gui.para.long"_tr(), "10", mDefLong);
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

            auto eleLong = utils::getStringElement(elements, "long");
            if (!eleLong.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto _long = utils::tryGetInt(eleLong.value());

            auto eleTimes = utils::getStringElement(elements, "times");
            if (!eleTimes.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto times = utils::tryGetInt(eleTimes.value());

            auto eleInterval = utils::getStringElement(elements, "interval");
            if (!eleInterval.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto interval = utils::tryGetInt(eleInterval.value());

            if (!_long.has_value() || !times.has_value() || !interval.has_value()) {
                int opIndex = utils::findOpIndex(op, eleOp.value());
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                mDefLong     = eleLong.value();
                mDefTimes    = eleTimes.value();
                mDefInterval = eleInterval.value();
                mDefOp       = opIndex;
                return this->sendTo(player);
            }

            if (eleOp.value() == "gui.longAction.use"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupUse(&player, mGroup->mData.name, _long.value(), times.value(), interval.value())
                );
            if (eleOp.value() == "gui.longAction.destroy"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance()
                        .groupDestroy(&player, mGroup->mData.name, _long.value(), times.value(), interval.value())
                );
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
