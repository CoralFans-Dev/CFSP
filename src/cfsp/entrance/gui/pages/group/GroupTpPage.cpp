#include "GroupTpPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void GroupTpPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.tp.groupTitle"_tr());
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", mDefPos);
    utils::appendDimDropdown(form, mDefDim);
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto elePos = utils::getStringElement(elements, "targetpos");
            if (!elePos.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto targetPos = utils::tryGetVec3(elePos.value());

            auto dim = utils::getDimElement(elements);
            if (!dim.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                mDefDim = dim.value();
                mDefPos = elePos.value();
                return this->sendTo(player);
            }
            utils::sendResults(
                player,
                manager::CFSPManager::getInstance().groupTp(&player, mGroup->mData.name, targetPos.value(), dim.value())
            );
        }
    );
}

} // namespace coral_fans::cfsp::gui
