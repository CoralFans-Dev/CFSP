#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/ConFig.h"
#include "cfsp/core/fix/CFSPFixManager.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/helper/CFSPHelperManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/core/simPlayer/SimPlayerSaveData.h"
#include "cfsp/entrance/command/ComandManager.h"
#include "ll/api/Config.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/world/actor/player/Player.h"
#include <boost/filesystem.hpp>
#include <memory>
#include <optional>
#include <vector>


namespace coral_fans::cfsp::manager {
CFSPManager& CFSPManager::getInstance() {
    static CFSPManager instance;
    return instance;
}

config::Config& CFSPManager::getConfig() { return this->mConfig; }

config::PermissionConfig& CFSPManager::getPermissionConfig() { return this->mPermissionConfig; }

bool CFSPManager::getAutoJoin() { return this->mConfig.autoJoin; }

bool CFSPManager::getAutoRespawn() { return this->mConfig.autoRespawn; }

bool CFSPManager::getAutoDespawn() { return this->mConfig.autoDespawn; }

void CFSPManager::setAutoJoin(bool isOpen) { this->mConfig.autoJoin = isOpen; }

void CFSPManager::setAutoRespawn(bool isOpen) { this->mConfig.autoRespawn = isOpen; }

void CFSPManager::setAutoDespawn(bool isOpen) { this->mConfig.autoDespawn = isOpen; }

bool CFSPManager::tryCreateDiretory(const std::filesystem::path& basePath, const std::string& dir) {
    if (dir.empty()) return false;
    if (dir[0] == ' ' || dir.ends_with(' ')) return false;
    auto path = basePath / reinterpret_cast<const char8_t*>(dir.c_str());
    try {
        if (!std::filesystem::exists(basePath)) {
            std::filesystem::create_directories(basePath);
        }
        return std::filesystem::exists(path) || std::filesystem::create_directory(path);
    } catch (...) {
        return false;
    }
}

void CFSPManager::save() {
    ll::config::saveConfig(this->mConfig, CFSP::getInstance().getSelf().getConfigDir() / "config.json");
}

void CFSPManager::loadSpSaveData() {
    auto dir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
    if (!std::filesystem::exists(dir)) std::filesystem::create_directories(dir);
    std::vector<std::string> splist;
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            simulated_player::SimPlayerSaveData playerData;
            if (ll::config::loadConfig(playerData, path.path() / "data.json")
                && playerData.name
                       == std::string(reinterpret_cast<const char*>(path.path().filename().u8string().c_str()))) {
                if (playerData.isOnline && !this->mConfig.autoJoin) {
                    playerData.isOnline = false;
                    ll::config::saveConfig(playerData, path.path() / "data.json");
                }
                this->mOfflineSpMap[playerData.name] = std::make_shared<simulated_player::SimPlayer>(playerData);
                splist.emplace_back(playerData.name);
            }
        }
    }
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspOfflineSp", splist);
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspSplist", splist);
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspOnlineSp", {});
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspDeadSp", {});
}

void CFSPManager::loadGroupData() {
    auto dir = CFSP::getInstance().getSelf().getDataDir() / "group";
    if (!std::filesystem::exists(dir)) std::filesystem::create_directories(dir);
    std::vector<std::string> cfspGrouplist;
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            group::GroupData groupData;
            if (ll::config::loadConfig(groupData, path.path() / "data.json")
                && groupData.name
                       == std::string(reinterpret_cast<const char*>(path.path().filename().u8string().c_str()))) {
                bool isChange = false;
                std::erase_if(groupData.splist, [this, &isChange](const auto& spName) {
                    if (this->mOfflineSpMap.find(spName) == this->mOfflineSpMap.end()) {
                        isChange = true;
                        return true;
                    }
                    return false;
                });
                if (isChange) ll::config::saveConfig(groupData, path.path() / "data.json");
                this->mGroupMap[groupData.name] = std::make_shared<group::CFSPGroup>(groupData);
                cfspGrouplist.emplace_back(groupData.name);
            }
        }
    }
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspGroup", cfspGrouplist);
}

bool CFSPManager::init() {
    const auto& logger = cfsp::CFSP::getInstance().getSelf().getLogger();
    try {
        const auto& configFilePath = cfsp::CFSP::getInstance().getSelf().getConfigDir() / "config.json";
        if (!ll::config::loadConfig(manager::CFSPManager::getInstance().getConfig(), configFilePath)) {
            logger.warn("Cannot load configurations from {}", configFilePath);
            logger.info("Saving default configurations");
            if (!ll::config::saveConfig(manager::CFSPManager::getInstance().getConfig(), configFilePath)) {
                logger.error("Cannot save default configurations to {}", configFilePath);
                return false;
            }
        }
    } catch (...) {
        logger.error("Failed to load config.json. Please check the file!");
        return false;
    }
    try {
        const auto& configFilePath = cfsp::CFSP::getInstance().getSelf().getConfigDir() / "commandPermission.json";
        if (!ll::config::loadConfig(this->mPermissionConfig, configFilePath)) {
            logger.warn("Cannot load command permission from {}", configFilePath);
            logger.info("Saving default command permission");
            if (!ll::config::saveConfig(this->mPermissionConfig, configFilePath)) {
                logger.error("Cannot save default command permission to {}", configFilePath);
                return false;
            }
        }
        return true;
    } catch (...) {
        logger.error("Failed to load commandPermission.json. Please check the file!");
        return false;
    }
}

void CFSPManager::load() {
    loadSpSaveData();
    loadGroupData();
    if (this->mConfig.enabled) command::ComandManager::getInstance().registerCommand(this->mConfig.permission);
    helper::CFSPHelperManager::getInstance().SimPlayerHelperHook();
    fix::CFSPFixManager::getInstance().featureFix();
}

std::optional<std::shared_ptr<simulated_player::SimPlayer>> CFSPManager::tryGetCFSP(Player* sp) {
    if (!sp->isSimulatedPlayer()) return std::nullopt;
    auto it = mOnlineSpMap.find(*sp->mName);
    if (it != mOnlineSpMap.end() && it->second->mSaveData.uniqueId == sp->getOrCreateUniqueID().rawID)
        return it->second;
    else return std::nullopt;
}

std::optional<std::shared_ptr<simulated_player::SimPlayer>> CFSPManager::tryGetCFSP(std::string const& name) {
    if (auto it = this->mOnlineSpMap.find(name); it != this->mOnlineSpMap.end()) return it->second;
    if (auto it = this->mOfflineSpMap.find(name); it != this->mOfflineSpMap.end()) return it->second;
    return std::nullopt;
}

std::optional<std::shared_ptr<group::CFSPGroup>> CFSPManager::tryGetCFSPGroup(std::string const& name) {
    if (auto it = this->mGroupMap.find(name); it != this->mGroupMap.end()) return it->second;
    return std::nullopt;
}

std::vector<std::string> CFSPManager::getSpNamesSorted(const Player* player) {
    std::vector<std::string> res;
    std::string              uuid = player->getUuid().asString();
    for (auto i : mOnlineSpMap) {
        if (i.second->mSaveData.ownerUuid == uuid || i.second->mSaveData.permission.contains(uuid))
            res.emplace_back(i.first);
    }
    for (auto i : mOfflineSpMap) {
        if (i.second->mSaveData.ownerUuid == uuid || i.second->mSaveData.permission.contains(uuid))
            res.emplace_back(i.first);
    }
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<std::string> CFSPManager::getGroupNamesSorted(const Player* player) {
    std::vector<std::string> res;
    std::string              uuid = player->getUuid().asString();
    for (auto i : mGroupMap) {
        if (i.second->mData.ownerUuid == uuid || i.second->mData.permission.contains(uuid)) res.emplace_back(i.first);
    }
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<std::string> CFSPManager::getPublicSpNamesSorted() {
    std::vector<std::string> res;
    for (auto i : mOnlineSpMap) {
        if (i.second->mSaveData.permission.contains("")) res.emplace_back(i.first);
    }
    for (auto i : mOfflineSpMap) {
        if (i.second->mSaveData.permission.contains("")) res.emplace_back(i.first);
    }
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<std::string> CFSPManager::getAllSpNamesSorted() {
    std::vector<std::string> res;
    for (auto i : mOnlineSpMap) res.emplace_back(i.first);
    for (auto i : mOfflineSpMap) res.emplace_back(i.first);
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<std::string> CFSPManager::getAllGroupNamesSorted() {
    std::vector<std::string> res;
    for (auto i : mGroupMap) res.emplace_back(i.first);
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<std::string> CFSPManager::getCanBeAddedSpList(const Player* player) {
    std::vector<std::string> res;
    for (auto i : mOnlineSpMap)
        if (i.second->hasPermission(player, simulated_player::SimPlayerPermission::BeAddedToGroup))
            res.emplace_back(i.first);
    for (auto i : mOfflineSpMap)
        if (i.second->hasPermission(player, simulated_player::SimPlayerPermission::BeAddedToGroup))
            res.emplace_back(i.first);
    std::sort(res.begin(), res.end());
    return res;
}
} // namespace coral_fans::cfsp::manager