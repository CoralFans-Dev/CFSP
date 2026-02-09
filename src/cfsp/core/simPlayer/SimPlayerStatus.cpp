#include "SimPlayer.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/LayeredAbilities.h"

namespace coral_fans::cfsp::simulated_player {
base::OperateResult SimPlayer::sneaking(std::optional<bool> enable) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (enable.has_value()) this->mSimPlayer->setSneaking(enable.value());
    else this->mSimPlayer->setSneaking(!this->mSimPlayer->getStatusFlag(ActorFlags::Sneaking));
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::swimming(std::optional<bool> enable) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (enable.has_value()) enable.value() ? this->mSimPlayer->startSwimming() : this->mSimPlayer->stopSwimming();
    else
        this->mSimPlayer->getStatusFlag(ActorFlags::Swimming) ? this->mSimPlayer->stopSwimming()
                                                              : this->mSimPlayer->startSwimming();
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::flying(std::optional<bool> enable) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    auto& spAbilities = this->mSimPlayer->getAbilities();
    if (spAbilities.getAbility(AbilitiesIndex::MayFly).mValue->mBoolVal) [[likely]] {
        if (enable.has_value()) {
            if (spAbilities.setAbility(AbilitiesIndex::Flying, enable.value())) [[likely]]
                return base::OperateResult::success("manager.success.operate"_tr());
        } else if (spAbilities.setAbility(
                       AbilitiesIndex::Flying,
                       !spAbilities.getAbility(AbilitiesIndex::Flying).mValue->mBoolVal
                   ))
            return base::OperateResult::success("manager.success.operate"_tr());
    }
    return base::OperateResult::error("manager.fail.faileToSetFlying"_tr());
}

base::OperateResult SimPlayer::sprinting(std::optional<bool> enable) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (enable.has_value()) this->mSimPlayer->setSprinting(enable.value());
    else this->mSimPlayer->setSprinting(!this->mSimPlayer->getStatusFlag(ActorFlags::Sprinting));
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player