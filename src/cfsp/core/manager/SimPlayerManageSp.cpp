#include "SimPlayerManager.h"
#include "ll/api/i18n/I18n.h"
#include "mc/network/packet/TextPacket.h"


namespace coral_fans::cfsp::manager {
base::CommandResult SimPlayerManager::createSp(Player* player, std::string const& name, Vec3 const& pos, int dim) {
    using ll::i18n_literals::operator""_tr;
    // check: isSimulatedPlayer
    if (player && player->isSimulatedPlayer()) return base::CommandResult(base::CommandResult::Type::error, "");
    std::string spname = this->mConfig.namePrefix + name + this->mConfig.namePostfix;
    // check: already exist
    if (this->mOnlineSpMap.find(spname) != this->mOnlineSpMap.end()
        || this->mOfflineSpDataMap.find(spname) != this->mOfflineSpDataMap.end()) {
        return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.exist"_tr());
    }
    if (!isManager(player)) {
        // check: maxOnline
        if (this->mOnlineCount >= this->mConfig.maxOnline)
            return base::CommandResult(
                base::CommandResult::Type::error,
                "translate.simplayer.error.toomanyonline"_tr(std::to_string(this->mConfig.maxOnline))
            );
        // check: maxOnlinePerPlayer
        auto uuid = player->getUuid().asString();
        if (this->mOnlineCountPerPlayer[uuid] >= this->mConfig.maxOnlinePerPlayer)
            return base::CommandResult(
                base::CommandResult::Type::error,
                "translate.simplayer.error.toomanyonlineperplayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
            );
        // check: maxOwn
        unsigned long long count = 0;
        for (auto sp : this->mOnlineSpMap) {
            if (sp.second->mSaveData.ownerUuid == uuid) count++;
        }
        for (auto spdata : this->mOfflineSpDataMap) {
            if (spdata.second->ownerUuid == uuid) count++;
        }
        if (count >= this->mConfig.maxOwn)
            return base::CommandResult(base::CommandResult::Type::error, "translate.simplayer.error.toomanyown"_tr());
    }
}
} // namespace coral_fans::cfsp::manager