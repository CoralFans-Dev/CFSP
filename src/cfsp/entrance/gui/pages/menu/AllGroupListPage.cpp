#include "AllGroupListPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/pages/group/CreateGroupPage.h"
#include "cfsp/entrance/gui/pages/group/OperateGroupPage.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void AllGroupListPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.grouplist.title"_tr());
    for (auto groupName : manager::CFSPManager::getInstance().getAllGroupNamesSorted()) {
        form.appendButton(groupName, [groupName, self = shared_from_this()](Player& player) {
            auto group = manager::CFSPManager::getInstance().tryGetCFSPGroup(groupName);
            if (!group.has_value()) return base::OperateResult::error("command.fail.groupNoFind"_tr()).sendTo(player);
            std::make_shared<OperateGroupPage>(group.value(), self)->sendTo(player);
        });
    }
    form.appendButton("gui.grouplist.newgroup"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<CreateGroupPage>(self)->sendTo(player);
    });
    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
