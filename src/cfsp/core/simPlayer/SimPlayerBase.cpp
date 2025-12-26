#include "SimPlayer.h"
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

bool SimPlayer::hasPermission(const Player* player, SimPlayerPermission permission) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return true;
    if ((this->mSaveData.publicPermission & (uint)permission) == (uint)permission) return true;
    auto it = this->mSaveData.permission.find(uuid);
    return it != this->mSaveData.permission.end() && (it->second & (uint)permission) == (uint)permission;
}

uint SimPlayer::getPermission(Player* player) {
    auto uuid = player->getUuid().asString();
    if (this->mSaveData.ownerUuid == uuid) return (uint)-1;
    if (auto it = this->mSaveData.permission.find(uuid); it != this->mSaveData.permission.end())
        return (it->second | this->mSaveData.publicPermission);
    return this->mSaveData.publicPermission;
}
} // namespace coral_fans::cfsp::simulated_player