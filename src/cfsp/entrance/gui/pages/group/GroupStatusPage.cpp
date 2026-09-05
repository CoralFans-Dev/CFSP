#include "GroupStatusPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void GroupStatusPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.status.groupTitle"_tr());
    if (mPerm & (uint)group::GroupPermission::Sneaking)
        form.appendToggle("sneaking", "gui.status.sneaking"_tr(), false);
    if (mPerm & (uint)group::GroupPermission::Swimming)
        form.appendToggle("swimming", "gui.status.swimming"_tr(), false);
    if (mPerm & (uint)group::GroupPermission::Flying) form.appendToggle("flying", "gui.status.flying"_tr(), false);
    if (mPerm & (uint)group::GroupPermission::Sprinting)
        form.appendToggle("sprinting", "gui.status.sprinting"_tr(), false);
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (auto value = utils::getToggleElement(elements, "sneaking"); value.has_value())
                utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupSneaking(&player, mGroup->mData.name, value.value())
                );

            if (auto value = utils::getToggleElement(elements, "swimming"); value.has_value())
                utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupSwimming(&player, mGroup->mData.name, value.value())
                );

            if (auto value = utils::getToggleElement(elements, "flying"); value.has_value())
                utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupFlying(&player, mGroup->mData.name, value.value())
                );

            if (auto value = utils::getToggleElement(elements, "sprinting"); value.has_value())
                utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupSprinting(&player, mGroup->mData.name, value.value())
                );
        }
    );
}

} // namespace coral_fans::cfsp::gui
