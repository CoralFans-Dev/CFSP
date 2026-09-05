#include "GroupDropPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void GroupDropPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.inv.group.drop"_tr());
    form.appendInput("times", "gui.para.times"_tr(), "1", mDefTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", mDefInterval);
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto eleTimes = utils::getStringElement(elements, "times");
            if (!eleTimes.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto times = utils::tryGetInt(eleTimes.value());

            auto eleInterval = utils::getStringElement(elements, "interval");
            if (!eleInterval.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto interval = utils::tryGetInt(eleInterval.value());

            if (!times.has_value() || !interval.has_value()) {
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                mDefTimes    = eleTimes.value();
                mDefInterval = eleInterval.value();
                return this->sendTo(player);
            }

            utils::sendResults(
                player,
                manager::CFSPManager::getInstance()
                    .groupDrop(&player, mGroup->mData.name, times.value(), interval.value())
            );
        }
    );
}

} // namespace coral_fans::cfsp::gui
