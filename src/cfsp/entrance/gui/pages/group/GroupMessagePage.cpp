#include "GroupMessagePage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void GroupMessagePage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.message.groupTitle"_tr());
    std::vector<std::string> op;
    if (mPerm & (uint)group::GroupPermission::Chat) op.emplace_back("gui.message.chat"_tr());
    if (mPerm & (uint)group::GroupPermission::RunCmd) op.emplace_back("gui.message.runcmd"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op);
    form.appendInput("text", "gui.para.text"_tr());
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;

            auto eleOp = utils::getStringElement(elements, "operate");
            if (!eleOp.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto eleText = utils::getStringElement(elements, "text");
            if (!eleText.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (eleOp.value() == "gui.message.chat"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupChat(&player, mGroup->mData.name, eleText.value())
                );
            if (eleOp.value() == "gui.message.runcmd"_tr())
                return utils::sendResults(
                    player,
                    manager::CFSPManager::getInstance().groupRunCmd(&player, mGroup->mData.name, eleText.value())
                );
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
