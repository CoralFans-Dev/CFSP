#include "OperateGroupPage.h"
#include "GroupActionPage.h"
#include "GroupDeleteSpConfirmPage.h"
#include "GroupInfoPage.h"
#include "GroupInvOperatorPage.h"
#include "GroupLongActionPage.h"
#include "GroupLookPage.h"
#include "GroupMessagePage.h"
#include "GroupMovePage.h"
#include "GroupPermPage.h"
#include "GroupStatusPage.h"
#include "GroupTpPage.h"
#include "ManageSpInGroupPage.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void OperateGroupPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form      = ll::form::SimpleForm(mGroup->mData.name);
    uint perm      = utils::calcGroupPerm(player, mGroup);
    bool ismanager = manager::CFSPManager::getInstance().isManager(&player);
    auto self      = shared_from_this();

    if (perm)
        form.appendButton("gui.operateGroup.groupInfo"_tr(), [this, self](Player& player) {
            std::make_shared<GroupInfoPage>(mGroup, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::AddSp || perm & (uint)group::GroupPermission::RmSp)
        form.appendButton("gui.operateGroup.manageSp"_tr(), [this, self, perm](Player& player) {
            std::make_shared<ManageSpInGroupPage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Spawn)
        form.appendButton("gui.operateGroup.online"_tr(), [gname = mGroup->mData.name](Player& player) {
            utils::sendResults(player, manager::CFSPManager::getInstance().groupSpawn(&player, gname));
        });
    if (perm & (uint)group::GroupPermission::Despawn)
        form.appendButton("gui.operateGroup.offline"_tr(), [gname = mGroup->mData.name](Player& player) {
            utils::sendResults(player, manager::CFSPManager::getInstance().groupDespawn(&player, gname));
        });
    if (perm & (uint)group::GroupPermission::Respawn)
        form.appendButton("gui.operateGroup.respawn"_tr(), [gname = mGroup->mData.name](Player& player) {
            utils::sendResults(player, manager::CFSPManager::getInstance().groupRespawn(&player, gname));
        });
    if (perm & (uint)group::GroupPermission::Tp)
        form.appendButton("gui.operateGroup.tp"_tr(), [this, self](Player& player) {
            std::make_shared<GroupTpPage>(
                mGroup,
                self,
                player.getDimensionId(),
                utils::posToString(utils::getLookOrFeetPos(player))
            )
                ->sendTo(player);
        });
    if (perm)
        form.appendButton("gui.operateGroup.inv"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupInvOperatorPage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::LookAt)
        form.appendButton("gui.operateGroup.lookat"_tr(), [this, self](Player& player) {
            std::make_shared<GroupLookPage>(mGroup, self, utils::posToString(utils::getLookOrFeetPos(player)))
                ->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::MoveTo || perm & (uint)group::GroupPermission::NavTo)
        form.appendButton("gui.operateGroup.move"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupMovePage>(mGroup, perm, self, utils::posToString(utils::getLookOrFeetPos(player)))
                ->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Attack || perm & (uint)group::GroupPermission::Build
        || perm & (uint)group::GroupPermission::Interact || perm & (uint)group::GroupPermission::Jump)
        form.appendButton("gui.operateGroup.perform"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupActionPage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Use || perm & (uint)group::GroupPermission::Destroy)
        form.appendButton("gui.operateGroup.perform2"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupLongActionPage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Stop)
        form.appendButton("gui.operateGroup.stop"_tr(), [gname = mGroup->mData.name](Player& player) {
            utils::sendResults(player, manager::CFSPManager::getInstance().groupStop(&player, gname));
        });
    if (perm & (uint)group::GroupPermission::Sneaking || perm & (uint)group::GroupPermission::Swimming
        || perm & (uint)group::GroupPermission::Flying || perm & (uint)group::GroupPermission::Sprinting)
        form.appendButton("gui.operateGroup.status"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupStatusPage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Chat || perm & (uint)group::GroupPermission::RunCmd)
        form.appendButton("gui.operateGroup.message"_tr(), [this, self, perm](Player& player) {
            std::make_shared<GroupMessagePage>(mGroup, perm, self)->sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Delete)
        form.appendButton("gui.operateGroup.rm"_tr(), [this, self](Player& player) {
            utils::showConfirmDialog(
                player,
                "gui.operateGroup.rm"_tr(),
                "gui.operateGroup.confirmRm"_tr(mGroup->mData.name),
                [this, self](Player& player) {
                    manager::CFSPManager::getInstance().groupDelete(&player, mGroup->mData.name).sendTo(player);
                },
                [this, self](Player& player) { this->sendTo(player); },
                [this, self](Player& player) { this->sendTo(player); }
            );
        });
    if (perm & (uint)group::GroupPermission::DeleteSp)
        form.appendButton("gui.operateGroup.deleteSp"_tr(), [this, self](Player& player) {
            utils::showConfirmDialog(
                player,
                "gui.operateGroup.deleteSp"_tr(),
                "gui.operateGroup.confirmDeleteSp"_tr(mGroup->mData.name),
                [this, self](Player& player) {
                    auto res = manager::CFSPManager::getInstance().groupDeleteSp(&player, mGroup->mData.name);
                    for (auto& perRes : res)
                        if (perRes.mType != base::OperateResult::Type::Swing) perRes.sendTo(player);
                    if (!mGroup->mData.splist.empty())
                        std::make_shared<GroupDeleteSpConfirmPage>(mGroup->mData.splist)->sendTo(player);
                },
                [this, self](Player& player) { this->sendTo(player); },
                [this, self](Player& player) { this->sendTo(player); }
            );
        });
    if (mGroup->mData.ownerUuid == player.getUuid().asString() || ismanager)
        form.appendButton("gui.operateGroup.perm"_tr(), [this, self](Player& player) {
            std::make_shared<GroupPermPage>(mGroup, self)->sendTo(player);
        });

    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
