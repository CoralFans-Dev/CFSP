#include "CreateSpPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void CreateSpPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.createSp.title"_tr());
    form.appendInput("name", "gui.createSp.name"_tr(), "", mDefName);
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", mDefPos);
    utils::appendDimDropdown(form, mDefDim);
    form.appendDropdown(
        "lockUniqueId",
        "gui.createSp.lockUniqueId"_tr(),
        std::vector<std::string>{"base.yesOrNo.yes"_tr(), "base.yesOrNo.no"_tr()},
        mDefLockUniqueId
    );
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (cancelReason.has_value()) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto name = utils::getStringElement(elements, "name");
            if (!name.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto elePos = utils::getStringElement(elements, "targetpos");
            if (!elePos.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto targetPos = utils::tryGetVec3(elePos.value());

            auto lockUniqueId = utils::getYesNoElement(elements, "lockUniqueId");
            if (!lockUniqueId.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto dim = utils::getDimElement(elements);
            if (!dim.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                mDefDim          = dim.value();
                mDefPos          = elePos.value();
                mDefName         = name.value();
                mDefLockUniqueId = lockUniqueId.value();
                return this->sendTo(player);
            }
            manager::CFSPManager::getInstance()
                .spCreate(&player, name.value(), targetPos.value(), dim.value(), lockUniqueId.value())
                .sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
