#include "AllSpListPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "cfsp/entrance/gui/pages/sp/CreateSpPage.h"
#include "cfsp/entrance/gui/pages/sp/OperateSpPage.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void AllSpListPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.managerSpList.title"_tr());
    for (auto spn : manager::CFSPManager::getInstance().getAllSpNamesSorted()) {
        form.appendButton(spn, [spn, self = shared_from_this()](Player& player) {
            auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(spn);
            if (!cfsp.has_value()) return base::OperateResult::error("command.fail.spNoFind"_tr()).sendTo(player);
            std::make_shared<OperateSpPage>(cfsp.value(), self)->sendTo(player);
        });
    }
    form.appendButton("gui.managerSpList.newsp"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<CreateSpPage>(self, player.getDimensionId(), utils::posToString(player.getFeetPos()))
            ->sendTo(player);
    });
    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
