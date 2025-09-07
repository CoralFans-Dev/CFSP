#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include "mc/scripting/modules/gametest/ScriptNavigationResult.h"

namespace coral_fans::cfsp::simulated_player {
base::OperateResult SimPlayer::moveTo(Vec3 const& pos) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    this->mSimPlayer->simulateMoveToLocation(pos, 4.3f, false);
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::navTo(Vec3 const& pos) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    this->mSimPlayer->simulateNavigateToLocation(pos, 4.3f);
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::tp(Vec3 const& pos, std::optional<int> dimId) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (!dimId.has_value()) this->mSimPlayer->teleport(pos, this->mSimPlayer->getDimensionId());
    else this->mSimPlayer->teleport(pos, dimId.value());
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player