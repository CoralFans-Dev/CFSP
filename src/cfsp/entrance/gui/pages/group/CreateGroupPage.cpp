#include "CreateGroupPage.h"
#include "ManageSpInGroupPage.h"
#include "OperateGroupPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void CreateGroupPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.managesp.title"_tr());
    form.appendInput("name", "gui.creategroup.title"_tr());
    form.sendTo(
        player,
        [self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (cancelReason.has_value()) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto name = utils::getStringElement(elements, "name");
            if (!name.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto res = manager::CFSPManager::getInstance().groupCreate(&player, name.value());
            res.sendTo(player);
            if (res) {
                auto group = manager::CFSPManager::getInstance().tryGetCFSPGroup(name.value()).value();
                std::make_shared<ManageSpInGroupPage>(group, (uint)-1, std::make_shared<OperateGroupPage>(group))
                    ->sendTo(player);
            }
        }
    );
}

} // namespace coral_fans::cfsp::gui
