#include "SimPlayer.h"
#include "cfsp/base/Schedule.h"
#include "cfsp/core/helper/CFSPHelperManager.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/gamemode/GameMode.h"
#include "mc/world/gamemode/InteractionResult.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/phys/HitResult.h"
#include <memory>


namespace coral_fans::cfsp::simulated_player {
base::OperateResult SimPlayer::attack(int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid = base::Schedule::getInstance().getSchedule()->add(interval, [times, this](unsigned long long t) {
        if (!this->mSimPlayer) [[unlikely]]
            return false;
        this->mSimPlayer->simulateAttack();
        return !times || t < (unsigned long long)times - 1;
    });
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::build(int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid = base::Schedule::getInstance().getSchedule()->add(interval, [times, this](unsigned long long t) {
        if (!this->mSimPlayer) [[unlikely]]
            return false;
        const auto& hit = this->mSimPlayer->traceRay(5.25f);
        if (hit.mType == HitResultType::Tile) {
            helper::CFSPHelperManager::getInstance().buildMutex = true;
            [[maybe_unused]] InteractionResult tem              = this->mSimPlayer->mGameMode->useItemOn(
                this->mSimPlayer->mInventory->mInventory->mItems.get()[0],
                hit.mBlock,
                hit.mFacing,
                hit.mPos,
                &this->mSimPlayer->getDimensionBlockSource().getBlock(hit.mBlock),
                true
            );
            helper::CFSPHelperManager::getInstance().buildMutex = false;
        }
        return !times || t < (unsigned long long)times - 1;
    });
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::interact(int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid = base::Schedule::getInstance().getSchedule()->add(interval, [times, this](unsigned long long t) {
        if (!this->mSimPlayer) [[unlikely]]
            return false;
        this->mSimPlayer->simulateInteract();
        return !times || t < (unsigned long long)times - 1;
    });
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::jump(int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid = base::Schedule::getInstance().getSchedule()->add(interval, [times, this](unsigned long long t) {
        if (!this->mSimPlayer) [[unlikely]]
            return false;
        this->mSimPlayer->simulateJump();
        return !times || t < (unsigned long long)times - 1;
    });
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::use(int _long, int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (_long < 1) _long = 0;
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid =
        base::Schedule::getInstance().getSchedule()->add(1, [_long, times, interval, this](unsigned long long t) {
            if (!this->mSimPlayer) [[unlikely]]
                return false;
            if (!_long && t >= (unsigned long long)interval) {
                this->mSimPlayer->simulateUseItem();
                return true;
            }
            auto index = t % (interval + _long);
            if (t && !index) {
                // simPlayer->simulateStopUsingItem();
                this->mSimPlayer->releaseUsingItem();
                return (t / (interval + _long)) != (unsigned long long)times;
            }
            if (index >= (unsigned long long)interval) this->mSimPlayer->simulateUseItem();
            return true;
        });
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::destroy(int _long, int times, int interval) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    if (this->mSimPlayer->isDead()) [[unlikely]]
        return base::OperateResult::error("manager.fail.spIsDead"_tr());
    if (!this->isFree()) return base::OperateResult::error("manager.fail.spIsBusy"_tr());
    if (_long < 1) _long = 0;
    if (times < 0) times = 0;
    if (interval < 1) interval = 1;
    this->mTaskid =
        base::Schedule::getInstance().getSchedule()->add(1, [_long, times, interval, this](unsigned long long t) {
            if (!this->mSimPlayer) [[unlikely]]
                return false;
            if (!_long && t >= (unsigned long long)interval) {
                this->mSimPlayer->simulateDestroyLookAt();
                return true;
            }
            auto index = t % (interval + _long);
            if (t && !index) {
                this->mSimPlayer->simulateStopDestroyingBlock();
                return t / (interval + _long) != (unsigned long long)times;
            }
            if (index >= (unsigned long long)interval) {
                const auto& hit = this->mSimPlayer->traceRay(5.25f);
                if (hit.mType == HitResultType::Tile) {
                    this->mSimPlayer->simulateDestroyLookAt();
                }
            }
            return true;
        });
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player