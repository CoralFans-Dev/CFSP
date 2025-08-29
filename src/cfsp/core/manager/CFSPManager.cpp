#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/ConFig.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayerSaveData.h"
#include "cfsp/entrance/command/ComandManager.h"
#include "ll/api/Config.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
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

std::shared_ptr<timewheel::TimeWheel> CFSPManager::getSchedule() { return this->mScheduler; }


void CFSPManager::loadSpSaveData() {
    auto                     dir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
    std::vector<std::string> splist;
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            simulated_player::SimPlayerSaveData playData;
            if (ll::config::saveConfig(playData, dir / "data.json")) {
                this->mOfflineSpMap[playData.name] = std::make_shared<simulated_player::SimPlayer>(playData);
                splist.emplace_back(playData.name);
            }
        }
    }
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspname", splist);
}

void CFSPManager::loadGroupData() {
    auto                     dir = CFSP::getInstance().getSelf().getDataDir() / "group";
    std::vector<std::string> cfspGrouplist;
    for (auto const& path : std::filesystem::directory_iterator(dir)) {
        if (path.is_directory()) {
            group::CFSPGroup group;
            if (ll::config::loadConfig(group.mData, dir / "data.json")) {
                bool isChange = false;
                std::erase_if(group.mData.splist, [this, &isChange](const auto& spName) {
                    if (this->mOfflineSpMap.find(spName) == this->mOfflineSpMap.end()) {
                        isChange = true;
                        return true;
                    }
                    return false;
                });
                if (isChange) ll::config::saveConfig(group.mData, dir / "data.json");
                this->mGroupMap[group.mData.name] = std::make_shared<group::CFSPGroup>(group);
                cfspGrouplist.emplace_back(group.mData.name);
            }
        }
    }
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum("cfspgroupname", cfspGrouplist);
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
        return true;
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
    } catch (...) {
        logger.error("Failed to load commandPermission.json. Please check the file!");
        return false;
    }
    loadSpSaveData();
    loadGroupData();
    if (this->mConfig.enabled) command::ComandManager::getInstance().registerCommand(this->mConfig.permission);
}

bool CFSPManager::isAllowed(Player* player) {
    if (!player) return true;
    switch (this->mConfig.listType) {
    case coral_fans::cfsp::config::ListType::disabled:
        return true;
    case coral_fans::cfsp::config::ListType::blacklist:
        if (this->mConfig.list.find(player->mName) != this->mConfig.list.end()) return false;
        return true;
    case coral_fans::cfsp::config::ListType::whitelist:
        if (this->mConfig.list.find(player->mName) == this->mConfig.list.end()) return false;
        return true;
    }
}

bool CFSPManager::isManager(Player* player) {
    return !player // 当player==null时，为控制台在执行命令
        || player->getCommandPermissionLevel() >= this->mConfig.adminPermission
        || this->mConfig.superManagerList.contains(*player->mName);
}

base::OperateResult CFSPManager::canCreatePlayer(Player* player) {
    using ll::i18n_literals::operator""_tr;
    // check: isSimulatedPlayer
    if (player && player->isSimulatedPlayer()) return base::OperateResult();
    if (!this->mPermissionConfig.base.create.enabled)
        return base::OperateResult::error("manager.fail.funcUnabled"_tr());
    if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    bool isManager = this->isManager(player);
    if (!isManager && player->getCommandPermissionLevel() < this->mPermissionConfig.base.create.permission)
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    auto ownerUuid = player->getUuid().asString();
    if (isManager) return base::OperateResult::success();
    // check: maxOnline
    if (this->mOnlineCount >= this->mConfig.maxOnline)
        return base::OperateResult::error("manager.fail.tooManyOnline"_tr(std::to_string(this->mConfig.maxOnline)));
    // check: maxOnlinePerPlayer
    if (this->mOnlineCountPerPlayer[ownerUuid] >= this->mConfig.maxOnlinePerPlayer)
        return base::OperateResult::error(
            "manager.fail.tooManyOnlinePerPlayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
        );
    // check: maxOwn
    unsigned long long count = 0;
    for (auto sp : this->mOnlineSpMap) {
        if (sp.second->mSaveData.ownerUuid == ownerUuid) count++;
    }
    for (auto spdata : this->mOfflineSpMap) {
        if (spdata.second->mSaveData.ownerUuid == ownerUuid) count++;
    }
    if (count >= this->mConfig.maxOwn) return base::OperateResult::error("manager.fail.tooManyOwn"_tr());
    return base::OperateResult::success();
}

std::optional<std::shared_ptr<simulated_player::SimPlayer>> CFSPManager::tryGetCFSP(Player* sp) {
    if (!sp->isSimulatedPlayer()) return std::nullopt;
    auto it = mOnlineSpMap.find(*sp->mName);
    if (it == mOnlineSpMap.end() && it->second->mSaveData.uniqueId == sp->getOrCreateUniqueID().rawID)
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
} // namespace coral_fans::cfsp::manager