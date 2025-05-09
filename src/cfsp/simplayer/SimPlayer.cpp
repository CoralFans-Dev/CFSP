#include "cfsp/CFSP.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/Mod.h"
#include "cfsp/simplayer/CFSP.h"
#include "cfsp/simplayer/luaapi/SpLuaApi.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "mc/_HeaderOutputPredefine.h"
#include "mc/dataloadhelper/DataLoadHelper.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/Tag.h"
#include "mc/platform/UUID.h"
#include "mc/server/ServerInstance.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/server/commands/StopCommand.h"
#include "mc/world/Minecraft.h"
#include "mc/world/SimpleContainer.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/GameType.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/storage/LevelStorageManager.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>


namespace coral_fans::cfsp {

namespace sputils {

bool saveSpNbt(boost::shared_ptr<SimPlayerManager::SimPlayerInfo> sp, std::filesystem::path basePath) {
    if (!sp->simPlayer) return false;
    auto tag = std::make_unique<CompoundTag>();
    if (!sp->simPlayer->save(*tag)) return false;
    if (!tag) return false;
    std::ofstream f(basePath / "nbt", std::ios_base::out | std::ios_base::trunc);
    if (!f.is_open()) return false;
    f << tag->toSnbt(SnbtFormat::Minimize);
    f.close();
    return true;
}

bool loadSpNbt(boost::shared_ptr<SimPlayerManager::SimPlayerInfo> sp, std::filesystem::path basePath) {
    if (!sp->simPlayer) return false;
    if (!std::filesystem::exists(basePath / "nbt")) return false;
    std::ifstream f(basePath / "nbt");
    if (!f.is_open()) return false;
    std::string snbt;
    f >> snbt;
    f.close();
    try {
        DefaultDataLoadHelper helper;
        sp->simPlayer->load(CompoundTag::fromSnbt(snbt).value(), helper);
    } catch (...) {
        return false;
    }
    return true;
}

bool emptyInv(boost::shared_ptr<SimPlayerManager::SimPlayerInfo> sp) {
    if (!sp->simPlayer) return false;
    bool ender = true;
    auto ec    = sp->simPlayer->getEnderChestContainer();
    if (ec.has_value()) ender = ec->isEmpty();
    return sp->simPlayer->getInventory().isEmpty() && ender
        && sp->simPlayer->getOffhandSlot() == ItemStack::EMPTY_ITEM()
        && ActorEquipment::getArmorContainer(sp->simPlayer->getEntityContext()).isEmpty();
}

} // namespace sputils

void SimPlayerManager::refreshSoftEnum() {
    std::vector<std::string> spvals;
    std::vector<std::string> gvals;
    for (const auto& i : this->mNameSimPlayerMap) spvals.emplace_back(i.first);
    for (const auto& i : this->mGroupNameMap) gvals.emplace_back(i.first);
    ll::command::CommandRegistrar::getInstance().setSoftEnumValues("spname", spvals);
    ll::command::CommandRegistrar::getInstance().setSoftEnumValues("gname", gvals);
}

void SimPlayerManager::save() {
    auto& modself = CFSP::getInstance().getSelf();
    try {
        const auto& logger  = modself.getLogger();
        const auto& dataDir = modself.getDataDir() / "simplayer" / "data";
        // rebuild dir
        std::filesystem::create_directories(dataDir);
        // for each simplayer
        for (auto& [name, sp] : this->mNameSimPlayerMap) {
            if (!sp->simPlayer) {
                logger.debug("Save SimPlayer ({}) Data: SKIP", name);
                continue;
            }
            sp->offlinePosX      = sp->simPlayer->getFeetPos().x;
            sp->offlinePosY      = sp->simPlayer->getFeetPos().y;
            sp->offlinePosZ      = sp->simPlayer->getFeetPos().z;
            sp->offlineDim       = sp->simPlayer->getDimensionId();
            sp->offlineRotX      = sp->simPlayer->getRotation().x;
            sp->offlineRotY      = sp->simPlayer->getRotation().y;
            sp->offlineGameType  = magic_enum::enum_name(sp->simPlayer->getPlayerGameType());
            sp->offlineEmptyInv  = sputils::emptyInv(sp);
            const auto& basePath = dataDir / sp->xuid;
            std::filesystem::create_directories(basePath);
            // save inventory
            if (!sputils::saveSpNbt(sp, basePath))
                logger.error("Failed to save SimPlayer ({}) NBT: cannot save data to {}", name, basePath);
        }
        // save self
        std::ofstream file(dataDir / "SimPlayerManager", std::ios_base::out | std::ios_base::trunc);
        if (!file.is_open()) {
            logger.error("Failed to save SimPlayerManager: cannot open {}", dataDir / "SimPlayerManager");
            return;
        }
        boost::archive::binary_oarchive oa(file);
        oa << *this;
        file.close();
    } catch (std::exception& e) {
        modself.getLogger().error("In SimPlayerManager::save: {}", e.what());
    }
}

void SimPlayerManager::load() {
    auto& modself = CFSP::getInstance().getSelf();
    try {
        std::filesystem::create_directories(modself.getDataDir() / "simplayer" / "scripts");
        const auto& logger  = modself.getLogger();
        const auto& dataDir = modself.getDataDir() / "simplayer" / "data";
        // check: exist
        if (!std::filesystem::exists(dataDir)) {
            logger.debug("Failed to load SimPlayer Data: not exist");
            return;
        }
        // load self
        std::ifstream file(dataDir / "SimPlayerManager");
        if (!file.is_open()) {
            logger.error("Failed to load SimPlayerManager: cannot open {}", dataDir / "SimPlayerManager");
            return;
        }
        boost::archive::binary_iarchive ia(file);
        ia >> *this;
        file.close();
        // for each simplayer
        for (auto& [name, sp] : this->mNameSimPlayerMap) {
            int stat   = sp->status;
            sp->status = SimPlayerStatus::Offline;
            if (this->autojoin && stat != SimPlayerStatus::Offline) this->spawnSimPlayer(nullptr, name, {}, {});
        }
        this->refreshSoftEnum();
    } catch (std::exception& e) {
        modself.getLogger().error("In SimPlayerManager::load: {}", e.what());
    }
}

std::string SimPlayerManager::listSimPlayer() {
    using ll::i18n_literals::operator""_tr;
    std::string retstr = "translate.simplayer.info.online"_tr(this->mOnlineCount);
    for (const auto& [name, sp] : this->mNameSimPlayerMap)
        retstr += "translate.simplayer.info.simplayer"_tr(
            name,
            sp->xuid,
            sp->ownerUuid,
            magic_enum::enum_name(magic_enum::enum_cast<SimPlayerStatus>(sp->status).value_or(SimPlayerStatus::Offline)
            ),
            boost::algorithm::join(sp->groups, ", ")
        );
    return retstr;
}

std::string SimPlayerManager::listGroup() {
    using ll::i18n_literals::operator""_tr;
    std::string retstr;
    for (const auto& [gname, spnames] : this->mGroupNameMap)
        retstr += "translate.simplayer.info.group"_tr(
            gname,
            boost::algorithm::join(this->mGroupAdminMap[gname], ", "),
            boost::algorithm::join(spnames, ", ")
        );
    return retstr.substr(0, retstr.length() - 1);
}

std::pair<std::string, bool> SimPlayerManager::createGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (this->mGroupNameMap.size() >= mod().getConfig().simPlayer.maxGroup)
        return {"translate.simplayer.error.toomanygroup"_tr(), false};
    if (!(this->mGroupNameMap.contains(gname)) && !(this->mGroupAdminMap.contains(gname))) {
        this->mGroupNameMap.emplace(gname, std::unordered_set<std::string>{});
        this->mGroupAdminMap.emplace(gname, std::unordered_set<std::string>{player->getUuid().asString()});
        this->refreshSoftEnum();
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.exist"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::deleteGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto it      = this->mGroupAdminMap.find(gname);
    auto namesIt = this->mGroupNameMap.find(gname);
    // check: exist
    if (it != this->mGroupAdminMap.end() && namesIt != this->mGroupNameMap.end()) {
        // check: admin
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || it->second.find(player->getUuid().asString()) != it->second.end()) {
            // erase group in SimPlayerInfo
            if (namesIt != this->mGroupNameMap.end()) {
                for (const auto& i : namesIt->second) {
                    auto simIt = this->mNameSimPlayerMap.find(i);
                    if (simIt != this->mNameSimPlayerMap.end()) simIt->second->groups.erase(gname);
                }
            }
            // delete group in mGroupNameMap
            this->mGroupNameMap.erase(gname);
            // delete group in mGroupAdminMap
            this->mGroupAdminMap.erase(gname);
            // refresh
            this->refreshSoftEnum();
            // return
            return {"translate.simplayer.success"_tr(), true};
        }
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    }
    return {"translate.simplayer.error.notfound"_tr(), false};
}

std::pair<std::string, bool>
SimPlayerManager::addSpToGroup(Player* player, std::string const& gname, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    auto it      = this->mGroupAdminMap.find(gname);
    auto namesIt = this->mGroupNameMap.find(gname);
    auto spIt    = this->mNameSimPlayerMap.find(spname);
    // check: exist
    if (it != this->mGroupAdminMap.end() && namesIt != this->mGroupNameMap.end()
        && spIt != this->mNameSimPlayerMap.end()) {
        // check: admin
        auto uuid = player->getUuid().asString();
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || (it->second.find(uuid) != it->second.end() && spIt->second->ownerUuid == uuid)) {
            // add group to SimPlayerInfo
            auto gnameIt = spIt->second->groups.find(gname);
            if (gnameIt != spIt->second->groups.end()) return {"translate.simplayer.error.exist"_tr(), false};
            spIt->second->groups.emplace(gname);
            // add sp to mGroupNameMap
            auto nameIt = namesIt->second.find(spname);
            if (nameIt != namesIt->second.end()) return {"translate.simplayer.error.exist"_tr(), false};
            namesIt->second.emplace(spname);
            // return
            return {"translate.simplayer.success"_tr(), true};
        }
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    }
    return {"translate.simplayer.error.notfound"_tr(), false};
}

std::pair<std::string, bool>
SimPlayerManager::rmSpFromGroup(Player* player, std::string const& gname, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    auto it      = this->mGroupAdminMap.find(gname);
    auto namesIt = this->mGroupNameMap.find(gname);
    auto spIt    = this->mNameSimPlayerMap.find(spname);
    // check: exist
    if (it != this->mGroupAdminMap.end() && namesIt != this->mGroupNameMap.end()
        && spIt != this->mNameSimPlayerMap.end()) {
        // check: admin
        auto uuid = player->getUuid().asString();
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || (it->second.find(uuid) != it->second.end() && spIt->second->ownerUuid == uuid)) {
            // rm group from SimPlayerInfo
            auto gnameIt = spIt->second->groups.find(gname);
            if (gnameIt == spIt->second->groups.end()) return {"translate.simplayer.error.notfound"_tr(), false};
            spIt->second->groups.erase(gnameIt);
            // remove sp from mGroupNameMap
            auto nameIt = namesIt->second.find(spname);
            if (nameIt == namesIt->second.end()) return {"translate.simplayer.error.notfound"_tr(), false};
            namesIt->second.erase(nameIt);
            // return
            return {"translate.simplayer.success"_tr(), true};
        }
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    }
    return {"translate.simplayer.error.notfound"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::addAdminToGroup(Player* player, std::string const& gname, Player* obj) {
    using ll::i18n_literals::operator""_tr;
    auto it      = this->mGroupAdminMap.find(gname);
    auto namesIt = this->mGroupNameMap.find(gname);
    // check: exist
    if (it != this->mGroupAdminMap.end() && namesIt != this->mGroupNameMap.end() && obj) {
        // check: admin
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || it->second.find(player->getUuid().asString()) != it->second.end()) {
            // add player to mGroupAdminMap
            auto uuid   = obj->getUuid().asString();
            auto uuidIt = it->second.find(uuid);
            if (uuidIt != it->second.end()) return {"translate.simplayer.error.exist"_tr(), false};
            it->second.emplace(uuid);
            // return
            return {"translate.simplayer.success"_tr(), true};
        }
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    }
    return {"translate.simplayer.error.notfound"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::rmAdminFromGroup(Player* player, std::string const& gname, Player* obj) {
    using ll::i18n_literals::operator""_tr;
    auto it      = this->mGroupAdminMap.find(gname);
    auto namesIt = this->mGroupNameMap.find(gname);
    // check: exist
    if (it != this->mGroupAdminMap.end() && namesIt != this->mGroupNameMap.end() && obj) {
        auto pUuid = player->getUuid().asString();
        auto oUuid = obj->getUuid().asString();
        // check: admin
        if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
            || it->second.find(pUuid) != it->second.end()) {
            // check: self
            if (pUuid == oUuid) return {"translate.simplayer.error.rmself"_tr(), false};
            // rm player from mGroupAdminMap
            auto uuidIt = it->second.find(oUuid);
            if (uuidIt == it->second.end()) return {"translate.simplayer.error.notfound"_tr(), false};
            it->second.erase(uuidIt);
            // return
            return {"translate.simplayer.success"_tr(), true};
        }
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    }
    return {"translate.simplayer.error.notfound"_tr(), false};
}

std::pair<std::string, bool>
SimPlayerManager::spawnSimPlayer(Player* player, std::string const& name, Vec3 const& pos, Vec2 const& rot) {
    using ll::i18n_literals::operator""_tr;
    auto& cfspmod = mod();
    // check: spawn count
    if (this->mSpawnCount >= cfspmod.getConfig().simPlayer.maxSpawnCount)
        return {"translate.simplayer.error.maxspawn"_tr(), false};
    auto spname = name;
    bool rejoin = false;
    // check: already exist
    auto spIt = this->mNameSimPlayerMap.find(spname);
    if (spIt != this->mNameSimPlayerMap.end()) {
        if (spIt->second->status != SimPlayerStatus::Offline) return {"translate.simplayer.error.exist"_tr(), false};
        else rejoin = true;
    } else {
        spname = cfspmod.getConfig().simPlayer.namePrefix + name + cfspmod.getConfig().simPlayer.namePostfix;
        spIt   = this->mNameSimPlayerMap.find(spname);
        if (spIt != this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.exist"_tr(), false};
    }
    // check: maxOnline
    if (this->mOnlineCount >= cfspmod.getConfig().simPlayer.maxOnline)
        return {"translate.simplayer.error.toomanyonline"_tr(), false};
    // check: maxOwn
    if (!rejoin) {
        auto ownerNameMapIt = this->mOwnerNameMap.find(player->getUuid().asString());
        if (ownerNameMapIt != this->mOwnerNameMap.end()
            && ownerNameMapIt->second.size() >= cfspmod.getConfig().simPlayer.maxOwn)
            return {"translate.simplayer.error.toomanyown"_tr(), false};
    }
    // create
    auto mc = ll::service::getMinecraft();
    if (!mc) return {"translate.simplayer.error.cannotcreate"_tr(), false};
    auto serverNetworkHandler = mc->getServerNetworkHandler();
    if (!serverNetworkHandler) return {"translate.simplayer.error.cannotcreate"_tr(), false};
    if (rejoin) {
        Vec3  offlinePos{spIt->second->offlinePosX, spIt->second->offlinePosY, spIt->second->offlinePosZ};
        Vec2  offlineRot{spIt->second->offlineRotX, spIt->second->offlineRotY};
        auto* simPlayer = SimulatedPlayer::create(
            spname,
            offlinePos,
            spIt->second->offlineDim,
            serverNetworkHandler,
            spIt->second->xuid
        );
        if (!simPlayer) return {"translate.simplayer.error.cannotcreate"_tr(), false};
        simPlayer->setPlayerGameType(
            magic_enum::enum_cast<GameType>(spIt->second->offlineGameType).value_or(GameType::WorldDefault)
        );
        simPlayer->teleport(offlinePos, spIt->second->offlineDim, offlineRot);
        simPlayer->simulateLookAt(
            simPlayer->getPosition() + Vec3::directionFromRotation(offlineRot),
            ::sim::LookDuration{2}
        );
        spIt->second->status    = SimPlayerStatus::Alive;
        spIt->second->simPlayer = simPlayer;
        spIt->second->scheduler = this->mScheduler;
        spIt->second->taskid    = 0;
        sputils::loadSpNbt(
            spIt->second,
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / "data" / spIt->second->xuid
        );
    } else {
        auto* simPlayer = SimulatedPlayer::create(
            spname,
            pos,
            player->getDimensionId(),
            serverNetworkHandler,
            "-" + std::to_string(std::hash<std::string>()(spname))
        );
        if (!simPlayer) return {"translate.simplayer.error.cannotcreate"_tr(), false};
        simPlayer->setPlayerGameType(player->getPlayerGameType());
        simPlayer->teleport(pos, player->getDimensionId(), rot);
        simPlayer->simulateLookAt(simPlayer->getPosition() + Vec3::directionFromRotation(rot), ::sim::LookDuration{2});
        // add to map
        this->mNameSimPlayerMap[spname] =
            boost::make_shared<SimPlayerInfo>(spname, player, pos, rot, simPlayer, this->mScheduler);
        this->mOwnerNameMap[player->getUuid().asString()].emplace(spname);
    }
    // add to softenum
    this->refreshSoftEnum();
    // add count
    ++this->mSpawnCount;
    ++this->mOnlineCount;
    // return
    return {"translate.simplayer.success"_tr(), true};
}

std::pair<std::string, bool> SimPlayerManager::spawnGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto adminIt = this->mGroupAdminMap.find(gname);
    auto it      = this->mGroupNameMap.find(gname);
    // check: exist
    if (it == this->mGroupNameMap.end() || adminIt == this->mGroupAdminMap.end())
        return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (adminIt->second.find(player->getUuid().asString()) == adminIt->second.end())
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    // run
    for (auto const& v : it->second) this->spawnSimPlayer(player, v, {}, {});
    // return
    return {"translate.simplayer.success"_tr(), true};
}

std::pair<std::string, bool>
SimPlayerManager::despawnSimPlayer(Player* player, std::string const& spname, bool noCheck) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid();
    auto it   = this->mNameSimPlayerMap.find(spname);
    // check: simplayer
    if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission || noCheck
        || uuid == it->second->ownerUuid) {
        // check: offline
        if (!noCheck && it->second->status == SimPlayerStatus::Offline)
            return {"translate.simplayer.error.statuserror"_tr(), false};
        // run
        if (it->second->simPlayer) {
            const auto& basePath = CFSP::getInstance().getSelf().getDataDir() / "simplayer" / "data" / it->second->xuid;
            sputils::saveSpNbt(it->second, basePath);
            it->second->offlinePosX     = it->second->simPlayer->getFeetPos().x;
            it->second->offlinePosY     = it->second->simPlayer->getFeetPos().y;
            it->second->offlinePosZ     = it->second->simPlayer->getFeetPos().z;
            it->second->offlineDim      = it->second->simPlayer->getDimensionId();
            it->second->offlineRotX     = it->second->simPlayer->getRotation().x;
            it->second->offlineRotY     = it->second->simPlayer->getRotation().y;
            it->second->offlineGameType = magic_enum::enum_name(it->second->simPlayer->getPlayerGameType());
            it->second->offlineEmptyInv = sputils::emptyInv(it->second);
            it->second->stop();
            it->second->simPlayer->simulateDisconnect();
            it->second->simPlayer = nullptr;
        };
        // change status
        it->second->status = SimPlayerStatus::Offline;
        --this->mOnlineCount;
        // return
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.permissiondenied"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::despawnGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto adminIt = this->mGroupAdminMap.find(gname);
    auto it      = this->mGroupNameMap.find(gname);
    // check: exist
    if (it == this->mGroupNameMap.end() || adminIt == this->mGroupAdminMap.end())
        return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (adminIt->second.find(player->getUuid().asString()) == adminIt->second.end())
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    // run
    for (auto const& v : it->second) this->despawnSimPlayer(player, v, true);
    // return
    return {"translate.simplayer.success"_tr(), true};
}

std::pair<std::string, bool> SimPlayerManager::rmSimPlayer(Player* player, std::string const& spname, bool noCheck) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid();
    auto it   = this->mNameSimPlayerMap.find(spname);
    // check: simplayer
    if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission || noCheck
        || uuid == it->second->ownerUuid) {
        // check: offline
        if (it->second->status != SimPlayerStatus::Offline)
            return {"translate.simplayer.error.statuserror"_tr(), false};
        // check: empty inventory
        if (!it->second->offlineEmptyInv) return {"translate.simplayer.error.notempty"_tr(), false};
        // remove
        std::filesystem::remove_all(
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / "data" / it->second->xuid
        );
        for (const auto& i : it->second->groups) this->mGroupNameMap[i].erase(spname);
        this->mOwnerNameMap[it->second->ownerUuid].erase(it->first);
        this->mNameSimPlayerMap.erase(it);
        this->refreshSoftEnum();
        // return
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.permissiondenied"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::rmGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto adminIt = this->mGroupAdminMap.find(gname);
    auto it      = this->mGroupNameMap.find(gname);
    // check: exist
    if (it == this->mGroupNameMap.end() || adminIt == this->mGroupAdminMap.end())
        return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (adminIt->second.find(player->getUuid().asString()) == adminIt->second.end())
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    // run
    auto set = this->mGroupNameMap[gname];
    for (const auto& i : set) this->rmSimPlayer(player, i, true);
    // return
    return {"translate.simplayer.success"_tr(), true};
}

std::pair<std::string, bool> SimPlayerManager::respawnSimPlayer(Player* player, std::string const& name, bool noCheck) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid();
    auto it   = this->mNameSimPlayerMap.find(name);
    // check: simplayer
    if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission || noCheck
        || uuid == it->second->ownerUuid) {
        // check: dead
        if (it->second->status != SimPlayerStatus::Dead) return {"translate.simplayer.error.statuserror"_tr(), false};
        // run
        if (it->second->simPlayer) it->second->simPlayer->simulateRespawn();
        // change status
        it->second->status = SimPlayerStatus::Alive;
        // return
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.permissiondenied"_tr(), false};
}

std::pair<std::string, bool> SimPlayerManager::respawnGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    auto adminIt = this->mGroupAdminMap.find(gname);
    auto it      = this->mGroupNameMap.find(gname);
    // check: exist
    if (it == this->mGroupNameMap.end() || adminIt == this->mGroupAdminMap.end())
        return {"translate.simplayer.error.notfound"_tr(), false};
    // check: admin
    if (adminIt->second.find(player->getUuid().asString()) == adminIt->second.end())
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    // run
    for (auto const& v : it->second) this->respawnSimPlayer(player, v, true);
    // return
    return {"translate.simplayer.success"_tr(), true};
}

void SimPlayerManager::setDead(std::string const& spname) {
    auto it = this->mNameSimPlayerMap.find(spname);
    // check: simplayer
    if (it == this->mNameSimPlayerMap.end()) return;
    it->second->status = SimPlayerStatus::Dead;
}

std::optional<boost::shared_ptr<SimPlayerManager::SimPlayerInfo>>
SimPlayerManager::fetchSimPlayer(std::string const& name) {
    if (auto it = this->mNameSimPlayerMap.find(name); it != this->mNameSimPlayerMap.end()) return it->second;
    else return std::nullopt;
}

SP_DEF(Stop, stop)
SP_DEF_WA(Sneaking, sneaking, bool)
SP_DEF_WA(Swimming, swimming, bool)
SP_DEF_TASK(Attack, attack)
SP_DEF_TASK_WA(Chat, chat, std::string const&)
SP_DEF_TASK(Destroy, destroy)
SP_DEF(DropSelectedItem, dropSelectedItem)
SP_DEF(DropInv, dropInv)
std::pair<std::string, bool> SimPlayerManager::simPlayerSwap(Player* player, std::string const& spname) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid();
    auto it   = this->mNameSimPlayerMap.find(spname);
    if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};
    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission
        || uuid == it->second->ownerUuid) {
        if (it->second->status != SimPlayerStatus::Alive) return {"translate.simplayer.error.statuserror"_tr(), false};
        if (it->second->simPlayer) it->second->swap(player);
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.permissiondenied"_tr(), false};
}
SP_DEF_TASK_WA(RunCmd, runCmd, std::string const&)
SP_DEF_WA(Select, select, int)
SP_DEF_TASK(Interact, interact)
SP_DEF_TASK(Jump, jump)
SP_DEF_TASK_WA(Use, useItem, int)
SP_DEF(Build, startBuild)
SP_DEF_WA(LookAt, lookAt, Vec3 const&)
SP_DEF_WA(MoveTo, moveTo, Vec3 const&)
SP_DEF_WA(NavTo, navigateTo, Vec3 const&)
std::pair<std::string, bool> SimPlayerManager::simPlayerScript(
    Player*            player,
    std::string const& spname,
    bool               noCheck,
    std::string const& arg,
    int                interval,
    std::string const& luaArg
) {
    using ll::i18n_literals::operator""_tr;
    auto uuid = player->getUuid();
    auto it   = this->mNameSimPlayerMap.find(spname);
    if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};
    if (player->getCommandPermissionLevel() >= mod().getConfig().simPlayer.adminPermission || noCheck
        || uuid == it->second->ownerUuid) {
        if (it->second->status != SimPlayerStatus::Alive) return {"translate.simplayer.error.statuserror"_tr(), false};
        if (!it->second->isFree()) return {"translate.simplayer.error.nonfree"_tr(), false};
        // run script
        if (it->second->simPlayer)
            return sputils::lua_api::execLuaScript(
                arg,
                interval,
                luaArg,
                it->second,
                CFSP::getInstance().getSelf().getDataDir() / "simplayer" / "scripts"
            );
        return {"translate.simplayer.success"_tr(), true};
    }
    return {"translate.simplayer.error.permissiondenied"_tr(), false};
}
std::pair<std::string, bool> SimPlayerManager::groupScript(
    Player*            player,
    std::string const& gname,
    std::string const& arg,
    int                interval,
    std::string const& luaArg
) {
    using ll::i18n_literals::operator""_tr;
    auto adminIt = this->mGroupAdminMap.find(gname);
    auto it      = this->mGroupNameMap.find(gname);
    if (it == this->mGroupNameMap.end() || adminIt == this->mGroupAdminMap.end())
        return {"translate.simplayer.error.notfound"_tr(), false};
    if (adminIt->second.find(player->getUuid().asString()) == adminIt->second.end())
        return {"translate.simplayer.error.permissiondenied"_tr(), false};
    for (auto const& v : it->second) this->simPlayerScript(player, v, true, arg, interval, luaArg);
    return {"translate.simplayer.success"_tr(), true};
}

LL_TYPE_INSTANCE_HOOK(CoralFansSimPlayerTickHook, ll::memory::HookPriority::Normal, Level, &Level::$tick, void) {
    origin();
    SimPlayerManager::getInstance().tick();
}

LL_TYPE_INSTANCE_HOOK(
    CoralFansSimPlayerDieEventHook,
    ll::memory::HookPriority::Normal,
    Player,
    &Player::$die,
    void,
    ActorDamageSource const& source
) {
    origin(source);
    if (this->isSimulatedPlayer()) {
        auto& manager = SimPlayerManager::getInstance();
        manager.setDead(this->getRealName());
        if (manager.getAutoRespawn()) manager.respawnSimPlayer(this, this->getRealName(), true);
    }
}

LL_TYPE_INSTANCE_HOOK(
    CoralFansSimPlayerServerStopSaveHook,
    ll::memory::HookPriority::Normal,
    StopCommand,
    &StopCommand::$execute,
    void,
    CommandOrigin const& arg1,
    CommandOutput&       arg2
) {
    SimPlayerManager::getInstance().save();
    origin(arg1, arg2);
}

LL_TYPE_INSTANCE_HOOK(
    CoralFansSimPlayerDataSaveHook,
    ll::memory::HookPriority::Normal,
    LevelStorageManager,
    &LevelStorageManager::saveGameData,
    void,
    std::chrono::steady_clock::time_point unknown
) {
    CFSP::getInstance().getSelf().getLogger().debug("call LevelStorageManager::saveGameData");
    SimPlayerManager::getInstance().save();
    origin(unknown);
}

void hookSimPlayer(bool hook) {
    if (hook) {
        CoralFansSimPlayerTickHook::hook();
        CoralFansSimPlayerDieEventHook::hook();
        CoralFansSimPlayerServerStopSaveHook::hook();
        CoralFansSimPlayerDataSaveHook::hook();
    } else {
        CoralFansSimPlayerTickHook::unhook();
        CoralFansSimPlayerDieEventHook::unhook();
        CoralFansSimPlayerServerStopSaveHook::unhook();
        CoralFansSimPlayerDataSaveHook::unhook();
    }
}

} // namespace coral_fans::cfsp