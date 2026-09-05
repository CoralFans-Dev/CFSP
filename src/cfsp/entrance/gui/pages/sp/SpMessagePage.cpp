#include "SpMessagePage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <vector>

namespace coral_fans::cfsp::gui {

void SpMessagePage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.message.spTitle"_tr());
    std::vector<std::string> op;
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Chat) op.emplace_back("gui.message.chat"_tr());
    if (mPerm & (uint)simulated_player::SimPlayerPermission::RunCmd) op.emplace_back("gui.message.runcmd"_tr());
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
                return manager::CFSPManager::getInstance()
                    .spChat(&player, mCfsp->mSaveData.name, eleText.value())
                    .sendTo(player);
            if (eleOp.value() == "gui.message.runcmd"_tr())
                return manager::CFSPManager::getInstance()
                    .spRunCmd(&player, mCfsp->mSaveData.name, eleText.value())
                    .sendTo(player);
            return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
