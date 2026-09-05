#include "ManagePage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void ManagePage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.manage.title"_tr());
    form.appendToggle("autojoin", "gui.manage.autojoin"_tr(), manager::CFSPManager::getInstance().getAutoJoin());
    form.appendToggle(
        "autorespawn",
        "gui.manage.autorespawn"_tr(),
        manager::CFSPManager::getInstance().getAutoRespawn()
    );
    form.appendToggle(
        "autodespawn",
        "gui.manage.autodespawn"_tr(),
        manager::CFSPManager::getInstance().getAutoDespawn()
    );
    form.sendTo(
        player,
        [self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (!cancelReason.has_value() && elements.has_value()
                && manager::CFSPManager::getInstance().isManager(&player)) {
                bool isChanged = false;
                if (auto value = utils::getToggleElement(elements, "autojoin");
                    value.has_value() && value.value() != manager::CFSPManager::getInstance().getAutoJoin()) {
                    manager::CFSPManager::getInstance().setAutoJoin(value.value());
                    isChanged = true;
                }
                if (auto value = utils::getToggleElement(elements, "autorespawn");
                    value.has_value() && value.value() != manager::CFSPManager::getInstance().getAutoRespawn()) {
                    manager::CFSPManager::getInstance().setAutoRespawn(value.value());
                    isChanged = true;
                }
                if (auto value = utils::getToggleElement(elements, "autodespawn");
                    value.has_value() && value.value() != manager::CFSPManager::getInstance().getAutoDespawn()) {
                    manager::CFSPManager::getInstance().setAutoDespawn(value.value());
                    isChanged = true;
                }
                if (isChanged) manager::CFSPManager::getInstance().save();
                base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
            }
        }
    );
}

} // namespace coral_fans::cfsp::gui
