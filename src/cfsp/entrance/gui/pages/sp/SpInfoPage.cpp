#include "SpInfoPage.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void SpInfoPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    ll::form::SimpleForm(
        "gui.info.spTitle"_tr(),
        manager::CFSPManager::getInstance().spInfo(&player, mCfsp->mSaveData.name).mInfo
    )
        .sendTo(
            player,
            [this, self = shared_from_this()](Player& player, int, ll::form::FormCancelReason cancelReason) {
                this->backOnCancel(player, cancelReason);
            }
        );
}

} // namespace coral_fans::cfsp::gui
