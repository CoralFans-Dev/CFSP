#include "GroupInvOperatorPage.h"
#include "GroupDropInvPage.h"
#include "GroupDropPage.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include <string>

namespace coral_fans::cfsp::gui {

void GroupInvOperatorPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.inv.group.title"_tr());
    auto self = shared_from_this();
    if (mPerm)
        form.appendButton("gui.inv.group.invInfo"_tr(), [this, self](Player& player) {
            std::string info = "";
            for (auto& perRes : manager::CFSPManager::getInstance().groupInvInfo(&player, mGroup->mData.name))
                info += perRes.mInfo;
            ll::form::SimpleForm("gui.inv.group.invInfo"_tr(), info)
                .sendTo(player, [this, self](Player& player, int, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        this->sendTo(player);
                });
        });
    if (mPerm & (uint)group::GroupPermission::Drop)
        form.appendButton("gui.inv.group.drop"_tr(), [this, self](Player& player) {
            std::make_shared<GroupDropPage>(mGroup, self)->sendTo(player);
        });
    if (mPerm & (uint)group::GroupPermission::DropInv)
        form.appendButton("gui.inv.group.dropinv"_tr(), [this, self](Player& player) {
            std::make_shared<GroupDropInvPage>(mGroup, self)->sendTo(player);
        });
    form.sendTo(player, [this, self](Player& player, int, ll::form::FormCancelReason cancelReason) {
        this->backOnCancel(player, cancelReason);
    });
}

} // namespace coral_fans::cfsp::gui
