#include "SpInvOperatorPage.h"
#include "SpDropInvPage.h"
#include "SpDropPage.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void SpInvOperatorPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.inv.sp.title"_tr());
    auto self = shared_from_this();
    if (mPerm)
        form.appendButton("gui.inv.sp.invinfo"_tr(), [this, self](Player& player) {
            ll::form::SimpleForm(
                "gui.inv.sp.invinfo"_tr(),
                manager::CFSPManager::getInstance().spInvInfo(&player, mCfsp->mSaveData.name).mInfo
            )
                .sendTo(player, [this, self](Player& player, int, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        this->sendTo(player);
                });
        });
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Swap)
        form.appendButton("gui.inv.sp.swap"_tr(), [spname = mCfsp->mSaveData.name](Player& player) {
            manager::CFSPManager::getInstance().spSwap(&player, spname).sendTo(player);
        });
    if (mCfsp->isFree()) {
        if (mPerm & (uint)simulated_player::SimPlayerPermission::Drop)
            form.appendButton("gui.inv.sp.drop"_tr(), [this, self](Player& player) {
                std::make_shared<SpDropPage>(mCfsp, self)->sendTo(player);
            });
        if (mPerm & (uint)simulated_player::SimPlayerPermission::DropInv)
            form.appendButton("gui.inv.sp.dropinv"_tr(), [this, self](Player& player) {
                std::make_shared<SpDropInvPage>(mCfsp, self)->sendTo(player);
            });
    }
    form.sendTo(player, [this, self](Player& player, int, ll::form::FormCancelReason cancelReason) {
        this->backOnCancel(player, cancelReason);
    });
}

} // namespace coral_fans::cfsp::gui
