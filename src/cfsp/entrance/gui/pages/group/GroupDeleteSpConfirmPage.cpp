#include "GroupDeleteSpConfirmPage.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void GroupDeleteSpConfirmPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto        it     = mConfirmList.begin();
    std::string spname = *it;
    mConfirmList.erase(it);
    utils::showConfirmDialog(
        player,
        "gui.operateGroup.deleteSp"_tr(),
        "gui.operateGroup.confirmDeleteSp2"_tr(spname),
        [this, self = shared_from_this(), spname](Player& player) {
            manager::CFSPManager::getInstance().spDelete(&player, spname, true, true).sendTo(player);
            if (!mConfirmList.empty()) this->sendTo(player);
        },
        [this, self = shared_from_this()](Player& player) {
            if (!mConfirmList.empty()) this->sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
