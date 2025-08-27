#include "SimPlayerManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/ConFig.h"
#include "cfsp/core/group/SimPlayerGroup.h"
#include "cfsp/core/simPlayer/SimPlayerSaveData.h"
#include "ll/api/Config.h"
#include <memory>
#include <optional>


namespace coral_fans::cfsp::manager {
SimPlayerManager& SimPlayerManager::getInstance() {
    static SimPlayerManager instance;
    return instance;
}

config::Config& SimPlayerManager::getConfig() { return this->mConfig; }

std::shared_ptr<timewheel::TimeWheel> SimPlayerManager::getSchedule() { return this->mScheduler; }

std::optional<std::shared_ptr<simulated_player::SimPlayer>> SimPlayerManager::tryGetCFSP(Player* sp) {
    if (!sp->isSimulatedPlayer()) return std::nullopt;
    auto it = mOnlineSpMap.find(*sp->mName);
    if (it == mOnlineSpMap.end() && it->second->mSaveData.uniqueId == sp->getOrCreateUniqueID().rawID)
        return it->second;
    else return std::nullopt;
}

void SimPlayerManager::loadSpSaveData() {
    auto dir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            simulated_player::SimPlayerSaveData playData;
            if (ll::config::saveConfig(playData, dir / "data.json"))
                this->mOfflineSpDataMap[playData.name] =
                    std::make_shared<simulated_player::SimPlayerSaveData>(playData);
        }
    }
}

void SimPlayerManager::loadGroupData() {
    auto dir = CFSP::getInstance().getSelf().getDataDir() / "group";
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            group::SimPlayerGroup group;
            if (ll::config::loadConfig(group.mData, dir / "data.json")) {
                bool isChange = false;
                std::erase_if(group.mData.splist, [this, &isChange](const auto& spName) {
                    if (this->mOfflineSpDataMap.find(spName) == this->mOfflineSpDataMap.end()) {
                        isChange = true;
                        return true;
                    }
                    return false;
                });
                if (isChange) ll::config::saveConfig(group.mData, dir / "data.json");
                this->mGroupMap[group.mData.name] = std::make_shared<group::SimPlayerGroup>(group);
            }
        }
    }
}

bool SimPlayerManager::loadData() {
    const auto& logger = cfsp::CFSP::getInstance().getSelf().getLogger();
    try {
        const auto& configFilePath = cfsp::CFSP::getInstance().getSelf().getConfigDir() / "config.json";
        if (!ll::config::loadConfig(manager::SimPlayerManager::getInstance().getConfig(), configFilePath)) {
            logger.warn("Cannot load configurations from {}", configFilePath);
            logger.info("Saving default configurations");
            if (!ll::config::saveConfig(manager::SimPlayerManager::getInstance().getConfig(), configFilePath)) {
                logger.error("Cannot save default configurations to {}", configFilePath);
                return false;
            }
        }
        return true;
    } catch (...) {
        logger.error("Failed to load config.json. Please check the file!");
        return false;
    }
    try {
        const auto& configFilePath = cfsp::CFSP::getInstance().getSelf().getConfigDir() / "commandPermission.json";
        if (!ll::config::loadConfig(this->mCommandPermission, configFilePath)) {
            logger.warn("Cannot load command permission from {}", configFilePath);
            logger.info("Saving default command permission");
            if (!ll::config::saveConfig(this->mCommandPermission, configFilePath)) {
                logger.error("Cannot save default command permission to {}", configFilePath);
                return false;
            }
        }
    } catch (...) {
        logger.error("Failed to load commandPermission.json. Please check the file!");
        return false;
    }
    loadSpSaveData();
    loadGroupData();
}

bool SimPlayerManager::isManager(Player* player) {
    return !player // 当player==null时，为控制台在执行命令
        || player->getCommandPermissionLevel() >= this->mConfig.adminPermission
        || this->mConfig.superManagerList.contains(*player->mName);
}
} // namespace coral_fans::cfsp::manager