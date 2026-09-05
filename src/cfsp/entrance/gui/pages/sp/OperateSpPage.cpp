#include "OperateSpPage.h"
#include "SpActionPage.h"
#include "SpInfoPage.h"
#include "SpInvOperatorPage.h"
#include "SpLongActionPage.h"
#include "SpLookPage.h"
#include "SpMessagePage.h"
#include "SpMovePage.h"
#include "SpPermPage.h"
#include "SpStatusPage.h"
#include "SpTpPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/LayeredAbilities.h"

namespace coral_fans::cfsp::gui {

void OperateSpPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form      = ll::form::SimpleForm(mCfsp->mSaveData.name);
    uint perm      = utils::calcSpPerm(player, mCfsp);
    bool ismanager = manager::CFSPManager::getInstance().isManager(&player);
    auto self      = shared_from_this();

    if (perm)
        form.appendButton("gui.operateSp.spinfo"_tr(), [this, self](Player& player) {
            std::make_shared<SpInfoPage>(mCfsp, self)->sendTo(player);
        });
    if (!mCfsp->mSimPlayer) {
        if (perm & (uint)simulated_player::SimPlayerPermission::Spawn)
            form.appendButton("gui.operateSp.online"_tr(), [spname = mCfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spSpawn(&player, spname).sendTo(player);
            });
    } else {
        if (perm & (uint)simulated_player::SimPlayerPermission::Despawn)
            form.appendButton("gui.operateSp.offline"_tr(), [spname = mCfsp->mSaveData.name](Player& player) {
                manager::CFSPManager::getInstance().spDespawn(&player, spname).sendTo(player);
            });
        if (mCfsp->mSimPlayer->isDead()) {
            if (perm & (uint)simulated_player::SimPlayerPermission::Respawn)
                form.appendButton("gui.operateSp.respawn"_tr(), [spname = mCfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spRespawn(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Tp)
                form.appendButton("gui.operateSp.tp"_tr(), [this, self](Player& player) {
                    std::make_shared<SpTpPage>(
                        mCfsp,
                        self,
                        player.getDimensionId(),
                        utils::posToString(utils::getLookOrFeetPos(player))
                    )
                        ->sendTo(player);
                });
            if (perm)
                form.appendButton("gui.operateSp.inv"_tr(), [this, self, perm](Player& player) {
                    std::make_shared<SpInvOperatorPage>(mCfsp, perm, self)->sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, self, perm](Player& player) {
                    std::make_shared<SpMessagePage>(mCfsp, perm, self)->sendTo(player);
                });
        } else {
            form.appendButton("gui.operateSp.inv"_tr(), [this, self, perm](Player& player) {
                std::make_shared<SpInvOperatorPage>(mCfsp, perm, self)->sendTo(player);
            });
            if (perm & (uint)simulated_player::SimPlayerPermission::Tp)
                form.appendButton("gui.operateSp.tp"_tr(), [this, self](Player& player) {
                    std::make_shared<SpTpPage>(
                        mCfsp,
                        self,
                        player.getDimensionId(),
                        utils::posToString(utils::getLookOrFeetPos(player))
                    )
                        ->sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::LookAt)
                form.appendButton("gui.operateSp.lookat"_tr(), [this, self](Player& player) {
                    if (!mCfsp->mSimPlayer)
                        return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                    if (player.getDimensionId() != mCfsp->mSimPlayer->getDimensionId())
                        std::make_shared<SpLookPage>(mCfsp, self)->sendTo(player);
                    else
                        std::make_shared<SpLookPage>(mCfsp, self, utils::posToString(utils::getLookOrFeetPos(player)))
                            ->sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::MoveTo
                || perm & (uint)simulated_player::SimPlayerPermission::NavTo)
                form.appendButton("gui.operateSp.move"_tr(), [this, self, perm](Player& player) {
                    if (!mCfsp->mSimPlayer)
                        return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
                    if (player.getDimensionId() != mCfsp->mSimPlayer->getDimensionId())
                        std::make_shared<SpMovePage>(mCfsp, perm, self)->sendTo(player);
                    else
                        std::make_shared<SpMovePage>(
                            mCfsp,
                            perm,
                            self,
                            utils::posToString(utils::getLookOrFeetPos(player))
                        )
                            ->sendTo(player);
                });
            if (mCfsp->isFree()) {
                if (perm & (uint)simulated_player::SimPlayerPermission::Attack
                    || perm & (uint)simulated_player::SimPlayerPermission::Build
                    || perm & (uint)simulated_player::SimPlayerPermission::Interact
                    || perm & (uint)simulated_player::SimPlayerPermission::Jump)
                    form.appendButton("gui.operateSp.perform"_tr(), [this, self, perm](Player& player) {
                        std::make_shared<SpActionPage>(mCfsp, perm, self)->sendTo(player);
                    });
                if (perm & (uint)simulated_player::SimPlayerPermission::Use
                    || perm & (uint)simulated_player::SimPlayerPermission::Destroy)
                    form.appendButton("gui.operateSp.perform2"_tr(), [this, self, perm](Player& player) {
                        std::make_shared<SpLongActionPage>(mCfsp, perm, self)->sendTo(player);
                    });
            }
            if (perm & (uint)simulated_player::SimPlayerPermission::Stop)
                form.appendButton("gui.operateSp.stop"_tr(), [spname = mCfsp->mSaveData.name](Player& player) {
                    manager::CFSPManager::getInstance().spStop(&player, spname).sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Sneaking
                || perm & (uint)simulated_player::SimPlayerPermission::Swimming
                || (perm & (uint)simulated_player::SimPlayerPermission::Flying
                    && mCfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
                || perm & (uint)simulated_player::SimPlayerPermission::Sprinting)
                form.appendButton("gui.operateSp.status"_tr(), [this, self, perm](Player& player) {
                    std::make_shared<SpStatusPage>(mCfsp, perm, self)->sendTo(player);
                });
            if (perm & (uint)simulated_player::SimPlayerPermission::Chat
                || perm & (uint)simulated_player::SimPlayerPermission::RunCmd)
                form.appendButton("gui.operateSp.message"_tr(), [this, self, perm](Player& player) {
                    std::make_shared<SpMessagePage>(mCfsp, perm, self)->sendTo(player);
                });
        }
    }
    if (perm & (uint)simulated_player::SimPlayerPermission::Delete)
        form.appendButton("gui.operateSp.rm"_tr(), [this, self](Player& player) {
            utils::showConfirmDialog(
                player,
                "gui.operateSp.rm"_tr(),
                "gui.operateSp.confirmRm"_tr(mCfsp->mSaveData.name),
                [this, self](Player& player) {
                    if (mCfsp->mSimPlayer) {
                        if (mCfsp->isEmptyInv())
                            return manager::CFSPManager::getInstance()
                                .spDelete(&player, mCfsp->mSaveData.name)
                                .sendTo(player);
                    } else if (mCfsp->checkInvEmptyForOfflineCFSP())
                        return manager::CFSPManager::getInstance()
                            .spDelete(&player, mCfsp->mSaveData.name)
                            .sendTo(player);
                    utils::showConfirmDialog(
                        player,
                        "gui.operateSp.rm"_tr(),
                        "gui.operateSp.confirmRm2"_tr(mCfsp->mSaveData.name),
                        [this, self](Player& player) {
                            manager::CFSPManager::getInstance()
                                .spDelete(&player, mCfsp->mSaveData.name, true)
                                .sendTo(player);
                        },
                        [this, self](Player& player) { this->sendTo(player); },
                        [this, self](Player& player) { this->sendTo(player); }
                    );
                },
                [this, self](Player& player) { this->sendTo(player); },
                [this, self](Player& player) { this->sendTo(player); }
            );
        });
    if (mCfsp->mSaveData.ownerUuid == player.getUuid().asString() || ismanager)
        form.appendButton("gui.operateSp.perm"_tr(), [this, self](Player& player) {
            std::make_shared<SpPermPage>(mCfsp, self)->sendTo(player);
        });

    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
