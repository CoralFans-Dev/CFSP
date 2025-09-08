#include "CFSPManager.h"
#include "cfsp/CFSP.h"
#include "cfsp/ConFig.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/fix/CFSPFixManager.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/helper/CFSPHelperManager.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/core/simPlayer/SimPlayerSaveData.h"
#include "cfsp/entrance/command/ComandManager.h"
#include "ll/api/Config.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
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
    auto path = basePath / dir;
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
                && playerData.name == path.path().filename()) {
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
                && groupData.name == path.path().filename()) {
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

bool CFSPManager::isAllowed(const Player* player) {
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
    return true;
}

bool CFSPManager::isManager(const Player* player) {
    return !player // 当player==null时，为控制台在执行命令
        || player->getCommandPermissionLevel() >= this->mConfig.adminPermission
        || this->mConfig.superManagerList.contains(*player->mName);
}

base::OperateResult CFSPManager::baseCheck(const Player* player, config::FuncStruct func) {
    using ll::i18n_literals::operator""_tr;
    if (player && player->isSimulatedPlayer()) [[unlikely]]
        return base::OperateResult::error();
    if (!func.enabled) [[unlikely]]
        return base::OperateResult::error("manager.fail.funcUnabled"_tr());
    if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    if (this->isManager(player)) return base::OperateResult::success();
    if (player->getCommandPermissionLevel() < func.permission)
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return base::OperateResult();
}

base::OperateResult CFSPManager::canCreatePlayer(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spCreate);
        checkResult.mType != base::OperateResult::Type::Swing)
        return checkResult;
    auto uuid = player->getUuid().asString();
    // check: maxOnline
    if (this->mOnlineSpMap.size() >= this->mConfig.maxOnline)
        return base::OperateResult::error("manager.fail.tooManyOnline"_tr(std::to_string(this->mConfig.maxOnline)));
    // check: maxOnlinePerPlayer
    unsigned long long spawnCount = 0, OwnCount = 0;
    for (auto sp : this->mOnlineSpMap) {
        if (sp.second->mSaveData.lastSpawnerUuid == uuid) spawnCount++;
        if (sp.second->mSaveData.ownerUuid == uuid) OwnCount++;
    }
    if (spawnCount >= this->mConfig.maxOnlinePerPlayer)
        return base::OperateResult::error(
            "manager.fail.tooManyOnlinePerPlayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
        );
    // check: maxOwn
    for (auto sp : this->mOfflineSpMap) {
        if (sp.second->mSaveData.ownerUuid == uuid) OwnCount++;
    }
    if (OwnCount >= this->mConfig.maxOwn) return base::OperateResult::error("manager.fail.tooManyOwnSp"_tr());
    return base::OperateResult::success();
}

base::OperateResult CFSPManager::canSpawnPlayer(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSpawn);
        checkResult.mType != base::OperateResult::Type::Swing)
        return checkResult;
    // check: maxOnline
    auto uuid = player ? player->getUuid().asString() : "";
    if (this->mOnlineSpMap.size() >= this->mConfig.maxOnline)
        return base::OperateResult::error("manager.fail.tooManyOnline"_tr(std::to_string(this->mConfig.maxOnline)));
    // check: maxOnlinePerPlayer
    unsigned long long spawnCount = 0;
    for (auto sp : this->mOnlineSpMap)
        if (sp.second->mSaveData.lastSpawnerUuid == uuid) spawnCount++;
    if (spawnCount >= this->mConfig.maxOnlinePerPlayer)
        return base::OperateResult::error(
            "manager.fail.tooManyOnlinePerPlayer"_tr(std::to_string(this->mConfig.maxOnlinePerPlayer))
        );
    return base::OperateResult::success();
}

base::OperateResult CFSPManager::canCreateGroup(const Player* player) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.groupCreate)) return checkResult;
    auto               uuid  = player->getUuid().asString();
    unsigned long long count = 0;
    for (auto group : this->mGroupMap) {
        if (group.second->mData.ownerUuid == uuid) count++;
    }
    if (count >= this->mConfig.maxGroup) return base::OperateResult::error("manager.fail.tooManyOwnGroup"_tr());
    return base::OperateResult::success();
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

uint CFSPManager::getSpBasePermissionMask() {
    uint perm = 0;
    if (this->mPermissionConfig.spSpawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Spawn;
    if (this->mPermissionConfig.spDespawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Despawn;
    if (this->mPermissionConfig.spRespawn.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Respawn;
    if (this->mPermissionConfig.spRm.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Rm;
    if (this->mPermissionConfig.spStop.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Stop;
    if (this->mPermissionConfig.spDrop.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Drop;
    if (this->mPermissionConfig.spDropInv.enabled) perm |= (uint)simulated_player::SimPlayerPermission::DropInv;
    if (this->mPermissionConfig.spSwap.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Swap;
    if (this->mPermissionConfig.spSneaking.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Sneaking;
    if (this->mPermissionConfig.spSwimming.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Swimming;
    if (this->mPermissionConfig.spFlying.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Flying;
    if (this->mPermissionConfig.spSprinting.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Sprinting;
    if (this->mPermissionConfig.spAttack.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Attack;
    if (this->mPermissionConfig.spBuild.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Build;
    if (this->mPermissionConfig.spInteract.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Interact;
    if (this->mPermissionConfig.spJump.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Jump;
    if (this->mPermissionConfig.spUse.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Use;
    if (this->mPermissionConfig.spDestroy.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Destroy;
    if (this->mPermissionConfig.spChat.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Chat;
    if (this->mPermissionConfig.spRunCmd.enabled) perm |= (uint)simulated_player::SimPlayerPermission::RunCmd;
    if (this->mPermissionConfig.spSelect.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Select;
    if (this->mPermissionConfig.spLookAt.enabled) perm |= (uint)simulated_player::SimPlayerPermission::LookAt;
    if (this->mPermissionConfig.spMoveTo.enabled) perm |= (uint)simulated_player::SimPlayerPermission::MoveTo;
    if (this->mPermissionConfig.spNavTo.enabled) perm |= (uint)simulated_player::SimPlayerPermission::NavTo;
    if (this->mPermissionConfig.spTp.enabled) perm |= (uint)simulated_player::SimPlayerPermission::Tp;
    return perm;
}

uint CFSPManager::getSpPermissionMask(CommandPermissionLevel level) {
    uint perm = 0;
    if (level >= this->mPermissionConfig.spSpawn.permission) perm |= (uint)simulated_player::SimPlayerPermission::Spawn;
    if (level >= this->mPermissionConfig.spDespawn.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Despawn;
    if (level >= this->mPermissionConfig.spRespawn.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Respawn;
    if (level >= this->mPermissionConfig.spRm.permission) perm |= (uint)simulated_player::SimPlayerPermission::Rm;
    if (level >= this->mPermissionConfig.spStop.permission) perm |= (uint)simulated_player::SimPlayerPermission::Stop;
    if (level >= this->mPermissionConfig.spDrop.permission) perm |= (uint)simulated_player::SimPlayerPermission::Drop;
    if (level >= this->mPermissionConfig.spDropInv.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::DropInv;
    if (level >= this->mPermissionConfig.spSwap.permission) perm |= (uint)simulated_player::SimPlayerPermission::Swap;
    if (level >= this->mPermissionConfig.spSneaking.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Sneaking;
    if (level >= this->mPermissionConfig.spSwimming.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Swimming;
    if (level >= this->mPermissionConfig.spFlying.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Flying;
    if (level >= this->mPermissionConfig.spSprinting.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Sprinting;
    if (level >= this->mPermissionConfig.spAttack.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Attack;
    if (level >= this->mPermissionConfig.spBuild.permission) perm |= (uint)simulated_player::SimPlayerPermission::Build;
    if (level >= this->mPermissionConfig.spInteract.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Interact;
    if (level >= this->mPermissionConfig.spJump.permission) perm |= (uint)simulated_player::SimPlayerPermission::Jump;
    if (level >= this->mPermissionConfig.spUse.permission) perm |= (uint)simulated_player::SimPlayerPermission::Use;
    if (level >= this->mPermissionConfig.spDestroy.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Destroy;
    if (level >= this->mPermissionConfig.spChat.permission) perm |= (uint)simulated_player::SimPlayerPermission::Chat;
    if (level >= this->mPermissionConfig.spRunCmd.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::RunCmd;
    if (level >= this->mPermissionConfig.spSelect.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::Select;
    if (level >= this->mPermissionConfig.spLookAt.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::LookAt;
    if (level >= this->mPermissionConfig.spMoveTo.permission)
        perm |= (uint)simulated_player::SimPlayerPermission::MoveTo;
    if (level >= this->mPermissionConfig.spNavTo.permission) perm |= (uint)simulated_player::SimPlayerPermission::NavTo;
    if (level >= this->mPermissionConfig.spTp.permission) perm |= (uint)simulated_player::SimPlayerPermission::Tp;
    return perm;
}

uint CFSPManager::getSpPermissionMask(std::optional<CommandPermissionLevel> level) {
    static uint baseMask = this->getSpBasePermissionMask();
    if (!level.has_value()) return baseMask;
    switch (level.value()) {
    case CommandPermissionLevel::Any:
        static uint maskAny = baseMask & getSpPermissionMask(level);
        return maskAny;
    case CommandPermissionLevel::GameDirectors:
        static uint maskGameDirectors = baseMask & getSpPermissionMask(level);
        return maskGameDirectors;
    case CommandPermissionLevel::Admin:
        static uint maskAdmin = baseMask & getSpPermissionMask(level);
        return maskAdmin;
    case CommandPermissionLevel::Host:
        static uint maskHost = baseMask & getSpPermissionMask(level);
        return maskHost;
    case CommandPermissionLevel::Owner:
        static uint maskOwner = baseMask & getSpPermissionMask(level);
        return maskOwner;
    case CommandPermissionLevel::Internal:
        static uint maskInternal = baseMask & getSpPermissionMask(level);
        return maskInternal;
    }
}
} // namespace coral_fans::cfsp::manager