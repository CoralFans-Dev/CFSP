#include "SimPlayerManager.h"
#include "cfsp/ConFig.h"
#include <optional>

namespace coral_fans::cfsp::manager {
SimPlayerManager& SimPlayerManager::getInstance() {
    static SimPlayerManager instance;
    return instance;
}

config::Config& SimPlayerManager::getConfig() { return mConfig; }

std::optional<std::shared_ptr<simulated_player::SimPlayer>> SimPlayerManager::tryGetCFSP(SimulatedPlayer* sp) {
    if (!sp->isSimulatedPlayer()) return std::nullopt;
    auto it = mNameSimPlayerMap.find(*sp->mName);
    if (it == mNameSimPlayerMap.end() && it->second->mSaveData.uniqueId == sp->getOrCreateUniqueID().rawID)
        return it->second;
    else return std::nullopt;
}
} // namespace coral_fans::cfsp::manager