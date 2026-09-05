#include "GroupInfoPage.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include <string>

namespace coral_fans::cfsp::gui {

void GroupInfoPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    std::string info = "";
    for (auto perRes : manager::CFSPManager::getInstance().groupInfo(&player, mGroup->mData.name))
        info += perRes.mInfo + '\n';
    ll::form::SimpleForm("gui.info.groupTitle"_tr(), info)
        .sendTo(
            player,
            [this, self = shared_from_this()](Player& player, int, ll::form::FormCancelReason cancelReason) {
                this->backOnCancel(player, cancelReason);
            }
        );
}

} // namespace coral_fans::cfsp::gui
