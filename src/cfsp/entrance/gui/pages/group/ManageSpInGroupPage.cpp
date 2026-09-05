#include "ManageSpInGroupPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <string>
#include <vector>

namespace coral_fans::cfsp::gui {

void ManageSpInGroupPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.manageSpInGroup.title"_tr());
    std::vector<std::string> splist;
    if (manager::CFSPManager::getInstance().isManager(&player))
        splist = manager::CFSPManager::getInstance().getAllSpNamesSorted();
    else splist = manager::CFSPManager::getInstance().getCanBeAddedSpList(&player);
    for (auto const& spname : splist) {
        bool isContain = mGroup->mData.splist.contains(spname);
        if (isContain) {
            if (mPerm & (uint)group::GroupPermission::RmSp) form.appendToggle(spname, spname, isContain);
        } else if (mPerm & (uint)group::GroupPermission::AddSp) form.appendToggle(spname, spname, isContain);
    }
    form.sendTo(
        player,
        [this,
         self = shared_from_this(),
         splist](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) {
                this->back(player);
                return;
            }
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            std::vector<std::string> addList, removeList;
            for (auto const& spname : splist)
                if (auto value = utils::getToggleElement(elements, spname); value.has_value()) {
                    if (value.value()) {
                        if (!mGroup->mData.splist.contains(spname)) addList.emplace_back(spname);
                    } else if (mGroup->mData.splist.contains(spname)) removeList.emplace_back(spname);
                }
            std::vector<base::OperateResult> res;
            if (mPerm & (uint)group::GroupPermission::AddSp)
                for (auto const& spname : addList)
                    res.emplace_back(
                        manager::CFSPManager::getInstance().groupAddSp(&player, mGroup->mData.name, spname)
                    );
            if (mPerm & (uint)group::GroupPermission::RmSp)
                for (auto const& spname : removeList)
                    res.emplace_back(
                        manager::CFSPManager::getInstance().groupRmSp(&player, mGroup->mData.name, spname)
                    );
            utils::sendResults(player, res);
        }
    );
}

} // namespace coral_fans::cfsp::gui
