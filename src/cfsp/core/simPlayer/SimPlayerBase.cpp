#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Schedule.h"
#include "ll/api/i18n/I18n.h"


namespace coral_fans::cfsp::simulated_player {
SimPlayer::SimPlayer(SimPlayerSaveData saveData, SimulatedPlayer* sp) {
    this->mSaveData  = saveData;
    this->mSimPlayer = sp;
}

std::string SimPlayer::getName() { return this->mSaveData.name; }

std::string SimPlayer::getXuid() { return this->mSaveData.xuid; }

bool SimPlayer::isOnline() { return this->mSimPlayer != nullptr; }

bool SimPlayer::isFree() {
    auto schedule = base::Schedule::getInstance().getSchedule();
    return !schedule->isRunning(this->mTaskid) && !schedule->isRunning(this->mScriptid);
}

base::OperateResult SimPlayer::hasPermission(Player* player, SimPlayerPermission permission) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return base::OperateResult::success();
    if (auto it = this->mSaveData.permission.find(uuid);
        it != this->mSaveData.permission.end() && ((uint)it->second & (uint)permission) == (uint)permission)
        return base::OperateResult::success();
    if (((uint)this->mSaveData.publicPermission & (uint)permission) == (uint)permission)
        return base::OperateResult::success();
    return base::OperateResult::error("manager.fail.permissionDenied"_tr());
}

SimPlayerPermission SimPlayer::getPermission(Player* player) {
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return (SimPlayerPermission)-1;
    if (auto it = this->mSaveData.permission.find(uuid); it != this->mSaveData.permission.end())
        return (SimPlayerPermission)((uint)it->second | (uint)this->mSaveData.publicPermission);
    return this->mSaveData.publicPermission;
}
} // namespace coral_fans::cfsp::simulated_player