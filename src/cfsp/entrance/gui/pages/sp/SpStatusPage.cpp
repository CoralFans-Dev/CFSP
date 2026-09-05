#include "SpStatusPage.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/LayeredAbilities.h"

namespace coral_fans::cfsp::gui {

void SpStatusPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    if (!mCfsp->mSimPlayer) return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);
    auto form = ll::form::CustomForm("gui.status.spTitle"_tr());
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Sneaking)
        form.appendToggle(
            "sneaking",
            "gui.status.sneaking"_tr(),
            mCfsp->mSimPlayer->getStatusFlag(ActorFlags::Sneaking)
        );
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Swimming)
        form.appendToggle(
            "swimming",
            "gui.status.swimming"_tr(),
            mCfsp->mSimPlayer->getStatusFlag(ActorFlags::Swimming)
        );
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Flying
        && mCfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
        form.appendToggle("flying", "gui.status.flying"_tr(), mCfsp->mSimPlayer->isFlying());
    if (mPerm & (uint)simulated_player::SimPlayerPermission::Sprinting)
        form.appendToggle(
            "sprinting",
            "gui.status.sprinting"_tr(),
            mCfsp->mSimPlayer->getStatusFlag(ActorFlags::Sprinting)
        );
    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!mCfsp->mSimPlayer)
                return base::OperateResult::error("manager.error.loseSimplayer"_tr()).sendTo(player);

            if (auto value = utils::getToggleElement(elements, "sneaking"); value.has_value()) {
                if (value.value())
                    manager::CFSPManager::getInstance().spSneaking(&player, mCfsp->mSaveData.name, true).sendTo(player);
                else
                    manager::CFSPManager::getInstance()
                        .spSneaking(&player, mCfsp->mSaveData.name, false)
                        .sendTo(player);
            }

            if (auto value = utils::getToggleElement(elements, "swimming"); value.has_value()) {
                if (value.value())
                    manager::CFSPManager::getInstance().spSwimming(&player, mCfsp->mSaveData.name, true).sendTo(player);
                else
                    manager::CFSPManager::getInstance()
                        .spSwimming(&player, mCfsp->mSaveData.name, false)
                        .sendTo(player);
            }

            if (mCfsp->mSimPlayer->getAbilities().getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal)
                if (auto value = utils::getToggleElement(elements, "flying"); value.has_value()) {
                    if (value.value())
                        manager::CFSPManager::getInstance()
                            .spFlying(&player, mCfsp->mSaveData.name, true)
                            .sendTo(player);
                    else
                        manager::CFSPManager::getInstance()
                            .spFlying(&player, mCfsp->mSaveData.name, false)
                            .sendTo(player);
                }

            if (auto value = utils::getToggleElement(elements, "sprinting"); value.has_value()) {
                if (value.value())
                    manager::CFSPManager::getInstance()
                        .spSprinting(&player, mCfsp->mSaveData.name, true)
                        .sendTo(player);
                else
                    manager::CFSPManager::getInstance()
                        .spSprinting(&player, mCfsp->mSaveData.name, false)
                        .sendTo(player);
            }
        }
    );
}

} // namespace coral_fans::cfsp::gui
