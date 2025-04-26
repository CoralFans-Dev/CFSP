#pragma once

#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/Utils.h"
#include "ll/api/service/Bedrock.h"
#include "magic_enum.hpp"
#include "mc/deps/core/math/Vec2.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/deps/core/utility/MCRESULT.h"
#include "mc/scripting/modules/gametest/ScriptNavigationResult.h"
#include "mc/scripting/modules/minecraft/ScriptFacing.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/server/commands/CommandContext.h"
#include "mc/server/commands/CommandVersion.h"
#include "mc/server/commands/MinecraftCommands.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/server/sim/LookDuration.h"
#include "mc/world/Minecraft.h"
#include "mc/world/SimpleContainer.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include "mc/world/attribute/AttributeInstance.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/block/actor/BlockActorType.h"
#include "mc/world/level/block/actor/ChestBlockActor.h"
#include "mc/world/phys/HitResult.h"
#include "mc/world/phys/HitResultType.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/version.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>


#define MANAGER_VERSION 2
#define INFO_VERSION    1

namespace coral_fans::cfsp {

class SimPlayerManager {
public:
    enum SimPlayerStatus : int {
        Offline = 0,
        Alive   = 1,
        Dead    = 2,
    };
    struct SimPlayerInfo {
        std::string                           name;
        std::string                           xuid;
        std::string                           ownerUuid;
        std::unordered_set<std::string>       groups;
        int                                   status;
        float                                 offlinePosX;
        float                                 offlinePosY;
        float                                 offlinePosZ;
        int                                   offlineDim;
        float                                 offlineRotX;
        float                                 offlineRotY;
        std::string                           offlineGameType;
        bool                                  offlineEmptyInv;
        SimulatedPlayer*                      simPlayer; // no-save
        std::shared_ptr<timewheel::TimeWheel> scheduler; // no-save
        unsigned long long                    taskid;    // no-save
        unsigned long long                    scriptid;  // no-save
        // construction
        SimPlayerInfo()
        : name(),
          xuid(),
          ownerUuid(),
          groups(),
          status(0),
          offlinePosX(0),
          offlinePosY(0),
          offlinePosZ(0),
          offlineDim(0),
          offlineRotX(0),
          offlineRotY(0),
          offlineGameType(),
          offlineEmptyInv(true),
          simPlayer(nullptr),
          scheduler(),
          taskid(0),
          scriptid(0) {}
        SimPlayerInfo(
            std::string const&                           name,
            Player*                                      player,
            Vec3                                         pos,
            Vec2                                         rot,
            SimulatedPlayer*                             simPlayer,
            std::shared_ptr<timewheel::TimeWheel> const& timeWheel
        )
        : name(name),
          xuid("-" + std::to_string(std::hash<std::string>()(name))),
          ownerUuid(player->getUuid().asString()),
          groups(),
          status(SimPlayerStatus::Alive),
          offlinePosX(pos.x),
          offlinePosY(pos.y),
          offlinePosZ(pos.z),
          offlineDim(player->getDimensionId()),
          offlineRotX(rot.x),
          offlineRotY(rot.y),
          offlineGameType(std::string{magic_enum::enum_name(player->getPlayerGameType())}),
          offlineEmptyInv(true),
          simPlayer(simPlayer),
          scheduler(timeWheel),
          taskid(0),
          scriptid(0) {}
        SimPlayerInfo(const SimPlayerInfo&)            = delete;
        SimPlayerInfo& operator=(const SimPlayerInfo&) = delete;
        // serialization
        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version) {
            if (version == INFO_VERSION) {
                ar & name;
                ar & xuid;
                ar & ownerUuid;
                ar & groups;
                ar & status;
                ar & offlinePosX;
                ar & offlinePosY;
                ar & offlinePosZ;
                ar & offlineDim;
                ar & offlineRotX;
                ar & offlineRotY;
                ar & offlineGameType;
                ar & offlineEmptyInv;
            }
        }
        // functions
        CFSP_API inline std::string getName() { return name; }
        CFSP_API inline std::string getXuid() { return xuid; }
        CFSP_API inline int         getStatus() { return status; }
        CFSP_API Vec3               getPos() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getPosition();
        }
        CFSP_API Vec3 getFeetPos() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getFeetPos();
        }
        CFSP_API BlockPos getStandingOn() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getBlockPosCurrentlyStandingOn(simPlayer);
        }
        CFSP_API Vec2 getRot() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getRotation();
        }
        CFSP_API int getHealth() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getHealth();
        }
        CFSP_API float getHunger() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->getAttribute(SimulatedPlayer::HUNGER()).getCurrentValue();
        }
        CFSP_API bool sneaking(bool enable) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return enable ? simPlayer->simulateSneaking() : simPlayer->simulateStopSneaking();
        }
        CFSP_API void swimming(bool enable) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            if (enable) {
                simPlayer->startSwimming();
                taskid = scheduler->add(1, [sp = this->simPlayer](unsigned long long) {
                    if (sp) sp->startSwimming();
                    return false;
                });
            } else {
                simPlayer->stopSwimming();
                taskid = scheduler->add(1, [sp = this->simPlayer](unsigned long long) {
                    if (sp) sp->stopSwimming();
                    return false;
                });
            }
        }
        CFSP_API bool attack() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            const auto& hit = simPlayer->traceRay(5.25f, true, false);
            if (hit) return simPlayer->simulateAttack(hit.getEntity());
            else return false;
        }
        CFSP_API void chat(std::string const& msg) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateChat(msg);
        }
        CFSP_API bool destroy() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            const auto& hit = simPlayer->traceRay(5.25f, false, true);
            if (hit)
                return simPlayer->simulateDestroyBlock(
                    hit.mBlock,
                    static_cast<ScriptModuleMinecraft::ScriptFacing>(hit.mFacing)
                );
            else return false;
        }
        CFSP_API bool dropSelectedItem() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->simulateDropSelectedItem();
        }
        CFSP_API bool dropInv() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            bool rst = true;
            if (simPlayer->getSelectedItem() != ItemStack::EMPTY_ITEM()) rst &= simPlayer->simulateDropSelectedItem();
            int   sel  = simPlayer->getSelectedItemSlot();
            auto& inv  = simPlayer->getInventory();
            int   size = inv.getContainerSize();
            for (int i = 0; i < size; ++i) {
                if (i == sel || inv.getItem(i) == ItemStack::EMPTY_ITEM()) continue;
                utils::swapItemInContainer(simPlayer, sel, i);
                rst &= dropSelectedItem();
            }
            return rst;
        }
        CFSP_API void swap(Player* player) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            if (!player) throw std::invalid_argument("Player is null");
            // get data
            auto&      spInv    = simPlayer->getInventory();
            auto&      spArmor  = ActorEquipment::getArmorContainer(simPlayer->getEntityContext());
            auto&      pInv     = player->getInventory();
            const auto pOffhand = player->getOffhandSlot();
            auto&      pArmor   = ActorEquipment::getArmorContainer(player->getEntityContext());
            auto       spEnder  = simPlayer->getEnderChestContainer();
            auto       pEnder   = player->getEnderChestContainer();
            // swap offhand
            player->setOffhandSlot(simPlayer->getOffhandSlot());
            simPlayer->setOffhandSlot(pOffhand);
            // swap inv
            int spInvSize = spInv.getContainerSize();
            if (spInvSize == pInv.getContainerSize())
                for (int i = 0; i < spInvSize; ++i) {
                    const auto spItem = spInv.getItem(i);
                    spInv.setItem(i, pInv.getItem(i));
                    pInv.setItem(i, spItem);
                }
            // swap armor
            int spArmorSize = spArmor.getContainerSize();
            if (spArmorSize == pArmor.getContainerSize())
                for (int i = 0; i < spArmorSize; ++i) {
                    const auto spItem = spArmor.getItem(i);
                    spArmor.setItem(i, pArmor.getItem(i));
                    pArmor.setItem(i, spItem);
                }
            // swap enderchest
            int spEnderSize = spEnder->getContainerSize();
            if (spEnder.has_value() && pEnder.has_value() && spEnderSize == pEnder->getContainerSize())
                for (int i = 0; i < spEnderSize; ++i) {
                    const auto spItem = spEnder->getItem(i);
                    spEnder->setItem(i, pEnder->getItem(i));
                    pEnder->setItem(i, spItem);
                }
            // refresh
            player->refreshInventory();
        }
        CFSP_API bool runCmd(std::string const& cmd) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            CommandContext ctx(
                cmd,
                std::make_unique<PlayerCommandOrigin>(PlayerCommandOrigin(*simPlayer)),
                CommandVersion::CurrentVersion()
            );
            auto mc = ll::service::getMinecraft();
            if (mc) {
                auto rst = mc->getCommands().executeCommand(ctx, false);
                return rst.isSuccess();
            }
            return false;
        }
        CFSP_API std::pair<BlockPos, bool> getBlockPosFromView() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            const auto& hit = simPlayer->traceRay(5.25f, false, true);
            return {hit.mBlock, hit.mType == HitResultType::Tile};
        }
        CFSP_API int searchInInvWithId(int id, int start = 0) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            auto& inv  = simPlayer->getInventory();
            int   size = inv.getContainerSize();
            for (int i = start; i < size; ++i)
                if (inv.getItem(i).getId() == id) return i;
            return -1;
        }
        CFSP_API int searchInInvWithName(std::string const& itemName, int start = 0) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            auto& inv  = simPlayer->getInventory();
            int   size = inv.getContainerSize();
            for (int i = start; i < size; ++i)
                if (utils::removeMinecraftPrefix(inv.getItem(i).getTypeName()) == itemName) return i;
            return -1;
        }
        CFSP_API bool selectSlot(int slot) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            if (slot < 0 || slot >= simPlayer->getInventory().getContainerSize()) return false;
            int sel = simPlayer->getSelectedItemSlot();
            utils::swapItemInContainer(simPlayer, sel, slot);
            return true;
        }
        CFSP_API bool select(int id) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            int sel    = simPlayer->getSelectedItemSlot();
            int target = searchInInvWithId(id);
            if (target == sel) target = searchInInvWithId(id, sel + 1);
            if (target == -1) return false;
            utils::swapItemInContainer(simPlayer, sel, target);
            return true;
        }
        CFSP_API const ItemStack& getItemFromInv(int slot) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            auto& inv = simPlayer->getInventory();
            return inv.getItem(slot);
        }
        CFSP_API bool interact() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->simulateInteract();
        }
        CFSP_API bool jump() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            return simPlayer->simulateJump();
        }
        CFSP_API void useItem(int delay) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateUseItemInSlot(simPlayer->getSelectedItemSlot());
            taskid = scheduler->add(delay, [sp = this->simPlayer](unsigned long long) {
                if (sp) sp->simulateStopUsingItem();
                return false;
            });
        }
        CFSP_API void startBuild() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateStartBuildInSlot(simPlayer->getSelectedItemSlot());
        }
        CFSP_API void lookAt(Vec3 const& pos) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateLookAt(pos, ::sim::LookDuration{2});
        }
        CFSP_API void moveTo(Vec3 const& pos) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateMoveToLocation(pos, 4.3f, true);
        }
        CFSP_API void navigateTo(Vec3 const& pos) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateNavigateToLocation(pos, 4.3f);
        }
        CFSP_API inline void cancelTask() { scheduler->cancel(taskid); }
        CFSP_API inline void cancelScript() { scheduler->cancel(scriptid); }
        CFSP_API bool        isTaskFree() {
            if (scheduler->isRunning(taskid)) return false;
            else {
                taskid = 0;
                return true;
            }
        }
        CFSP_API bool isScriptFree() {
            if (scheduler->isRunning(scriptid)) return false;
            else {
                scriptid = 0;
                return true;
            }
        }
        CFSP_API inline bool isFree() { return isTaskFree() && isScriptFree(); }
        CFSP_API void        stopAction() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            simPlayer->simulateStopBuild();
            simPlayer->simulateStopDestroyingBlock();
            simPlayer->simulateStopFlying();
            simPlayer->simulateStopInteracting();
            simPlayer->simulateStopMoving();
            simPlayer->simulateStopUsingItem();
            if (scheduler->isRunning(taskid)) cancelTask();
            taskid = 0;
        }
        CFSP_API void stop() {
            stopAction();
            if (scheduler->isRunning(scriptid)) cancelScript();
            scriptid = 0;
        }
        CFSP_API int getFirstEmptySlot() {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            auto& inv  = simPlayer->getInventory();
            int   size = inv.getContainerSize();
            for (int i = 0; i < size; ++i)
                if (inv.getItem(i) == ItemStack::EMPTY_ITEM()) return i;
            return -1;
        }
        enum ContainerOperationErrCode : int {
            Success        = 0,
            NoHit          = 1,
            NoContainer    = 2,
            NoBlockActor   = 3,
            CannotOpen     = 4,
            SlotOutOfRange = 5,
            NoEnoughSpace  = 6,
            NoItem         = 7
        };
        CFSP_API ContainerOperationErrCode trySwapSlotWithContainer(int invSlot, int targetSlot) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            using errc      = ContainerOperationErrCode;
            const auto& hit = simPlayer->traceRay(5.25f, false, true);
            if (!hit) return errc::NoHit;
            auto&       blockSource = simPlayer->getDimensionBlockSource();
            const auto& block       = blockSource.getBlock(hit.mBlock);
            if (!block.isContainerBlock()) return errc::NoContainer;
            auto* blockActor = blockSource.getBlockEntity(hit.mBlock);
            if (!blockActor) return errc::NoBlockActor;
            auto type = blockActor->getType();
            if (type == BlockActorType::Chest || type == BlockActorType::ShulkerBox) {
                auto* chestBlockActor = reinterpret_cast<ChestBlockActor*>(blockActor);
                if (!chestBlockActor->canOpen(blockSource)) return errc::CannotOpen;
            }
            auto* container = blockActor->getContainer();
            if (!container) return errc::NoContainer;
            auto& inv = simPlayer->getInventory();
            if (invSlot >= inv.getContainerSize() || targetSlot >= container->getContainerSize())
                return errc::SlotOutOfRange;
            auto item = container->getItem(targetSlot);
            container->setItem(targetSlot, inv.getItem(invSlot));
            inv.setItem(invSlot, item);
            return errc::Success;
        }
        CFSP_API ContainerOperationErrCode tryPutIntoContainer(int invSlot) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            using errc      = ContainerOperationErrCode;
            const auto& hit = simPlayer->traceRay(5.25f, false, true);
            if (!hit) return errc::NoHit;
            auto&       blockSource = simPlayer->getDimensionBlockSource();
            const auto& block       = blockSource.getBlock(hit.mBlock);
            if (!block.isContainerBlock()) return errc::NoContainer;
            auto* blockActor = blockSource.getBlockEntity(hit.mBlock);
            if (!blockActor) return errc::NoBlockActor;
            auto type = blockActor->getType();
            if (type == BlockActorType::Chest || type == BlockActorType::ShulkerBox) {
                auto* chestBlockActor = reinterpret_cast<ChestBlockActor*>(blockActor);
                if (!chestBlockActor->canOpen(blockSource)) return errc::CannotOpen;
            }
            auto* container = blockActor->getContainer();
            if (!container) return errc::NoContainer;
            auto& inv = simPlayer->getInventory();
            if (invSlot >= inv.getContainerSize()) return errc::SlotOutOfRange;
            int size = container->getContainerSize();
            for (int i = 0; i < size; ++i) {
                if (container->getItem(i) == ItemStack::EMPTY_ITEM()) {
                    container->setItem(i, inv.getItem(invSlot));
                    inv.setItem(invSlot, ItemStack::EMPTY_ITEM());
                    return errc::Success;
                }
            }
            return errc::NoEnoughSpace;
        }
        CFSP_API ContainerOperationErrCode tryGetFromContainerWithName(std::string const& typeName) {
            if (!simPlayer) throw std::invalid_argument("SimPlayer is null");
            using errc      = ContainerOperationErrCode;
            const auto& hit = simPlayer->traceRay(5.25f, false, true);
            if (!hit) return errc::NoHit;
            auto&       blockSource = simPlayer->getDimensionBlockSource();
            const auto& block       = blockSource.getBlock(hit.mBlock);
            if (!block.isContainerBlock()) return errc::NoContainer;
            auto* blockActor = blockSource.getBlockEntity(hit.mBlock);
            if (!blockActor) return errc::NoBlockActor;
            auto type = blockActor->getType();
            if (type == BlockActorType::Chest || type == BlockActorType::ShulkerBox) {
                auto* chestBlockActor = reinterpret_cast<ChestBlockActor*>(blockActor);
                if (!chestBlockActor->canOpen(blockSource)) return errc::CannotOpen;
            }
            auto* container = blockActor->getContainer();
            if (!container) return errc::NoContainer;
            auto& inv = simPlayer->getInventory();
            if (int emptySlot = getFirstEmptySlot(); emptySlot != -1) {
                int size = container->getContainerSize();
                for (int i = 0; i < size; ++i) {
                    const auto& item = container->getItem(i);
                    if (utils::removeMinecraftPrefix(item.getTypeName()) == typeName) {
                        inv.setItem(emptySlot, item);
                        container->setItem(i, ItemStack::EMPTY_ITEM());
                        return errc::Success;
                    }
                }
                return errc::NoItem;
            } else return errc::NoEnoughSpace;
        }
    };

private:
    std::unordered_map<std::string, boost::shared_ptr<SimPlayerInfo>> mNameSimPlayerMap;
    std::unordered_map<std::string, std::unordered_set<std::string>>  mOwnerNameMap;
    std::unordered_map<std::string, std::unordered_set<std::string>>  mGroupNameMap;
    std::unordered_map<std::string, std::unordered_set<std::string>>  mGroupAdminMap;
    unsigned long long                                                mOnlineCount; // no-save
    unsigned long long                                                mSpawnCount;  // no-save
    std::shared_ptr<timewheel::TimeWheel>                             mScheduler;   // no-save
    bool                                                              autorespawn;
    bool                                                              autojoin;

private:
    SimPlayerManager()
    : mOnlineCount(0),
      mSpawnCount(0),
      mScheduler(std::make_shared<timewheel::TimeWheel>(1200)),
      autorespawn(false),
      autojoin(false) {}
    ~SimPlayerManager() { this->mScheduler->clear(); }
    SimPlayerManager(const SimPlayerManager&);
    SimPlayerManager& operator=(const SimPlayerManager&);

public:
    CFSP_API static SimPlayerManager& getInstance() {
        static SimPlayerManager inst;
        return inst;
    }

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        if (version == MANAGER_VERSION) {
            ar & mNameSimPlayerMap;
            ar & mOwnerNameMap;
            ar & mGroupNameMap;
            ar & mGroupAdminMap;
            ar & autorespawn;
            ar & autojoin;
        }
    }

private:
    void refreshSoftEnum();

public:
    CFSP_API void save();
    CFSP_API void load();
    inline void   tick() { this->mScheduler->tick(); }

public:
    CFSP_API std::string listSimPlayer();
    CFSP_API std::string listGroup();

public:
    CFSP_API inline void setAutoRespawn(bool isopen) { this->autorespawn = isopen; }
    CFSP_API inline void setAutoJoin(bool isopen) { this->autojoin = isopen; }
    CFSP_API inline bool getAutoRespawn() { return this->autorespawn; }
    CFSP_API inline bool getAutoJoin() { return this->autojoin; }

public:
    CFSP_API std::pair<std::string, bool> createGroup(Player*, std::string const&);
    CFSP_API std::pair<std::string, bool> deleteGroup(Player*, std::string const&);
    CFSP_API std::pair<std::string, bool> addSpToGroup(Player*, std::string const&, std::string const&);
    CFSP_API std::pair<std::string, bool> rmSpFromGroup(Player*, std::string const&, std::string const&);
    CFSP_API std::pair<std::string, bool> addAdminToGroup(Player*, std::string const&, Player*);
    CFSP_API std::pair<std::string, bool> rmAdminFromGroup(Player*, std::string const&, Player*);

public:
    CFSP_API std::pair<std::string, bool> spawnSimPlayer(Player*, std::string const&, Vec3 const&, Vec2 const&);
    CFSP_API std::pair<std::string, bool> spawnGroup(Player*, std::string const&);
    CFSP_API std::pair<std::string, bool> despawnSimPlayer(Player*, std::string const&, bool);
    CFSP_API std::pair<std::string, bool> despawnGroup(Player*, std::string const&);
    CFSP_API std::pair<std::string, bool> rmSimPlayer(Player*, std::string const&, bool);
    CFSP_API std::pair<std::string, bool> rmGroup(Player*, std::string const&);
    CFSP_API std::pair<std::string, bool> respawnSimPlayer(Player*, std::string const&, bool);
    CFSP_API std::pair<std::string, bool> respawnGroup(Player*, std::string const&);

public:
    CFSP_API void setDead(std::string const&);

public:
    CFSP_API bool hasGroup(std::string const& gname) {
        return mGroupAdminMap.contains(gname) && mGroupNameMap.contains(gname);
    }
    CFSP_API bool isGroupAdmin(Player* player, std::string const& gname) {
        return hasGroup(gname) && mGroupAdminMap[gname].contains(player->getUuid().asString());
    }
    CFSP_API bool inGroup(std::string const& spname, std::string const& gname) {
        return hasGroup(gname) && mGroupNameMap[gname].contains(spname);
    }
    CFSP_API bool hasSimPlayer(std::string const& spname) { return mNameSimPlayerMap.contains(spname); }
    CFSP_API bool isSimPlayerOwner(Player* player, std::string const& spname) {
        return hasSimPlayer(spname) && mNameSimPlayerMap[spname]->ownerUuid == player->getUuid().asString();
    }
    CFSP_API std::optional<boost::shared_ptr<SimPlayerManager::SimPlayerInfo>> fetchSimPlayer(std::string const&);

public:
    SP_REG_DEF(Stop)
    SP_REG_DEF(Sneaking, bool)
    SP_REG_DEF(Swimming, bool)
    SP_REG_DEF(Attack, int, int)
    SP_REG_DEF(Chat, std::string const&, int, int)
    SP_REG_DEF(Destroy, int, int)
    SP_REG_DEF(DropSelectedItem)
    SP_REG_DEF(DropInv)
    CFSP_API std::pair<std::string, bool> simPlayerSwap(Player*, std::string const&);
    SP_REG_DEF(RunCmd, std::string const&, int, int)
    SP_REG_DEF(Select, int)
    SP_REG_DEF(Interact, int, int)
    SP_REG_DEF(Jump, int, int)
    SP_REG_DEF(Use, int, int, int)
    SP_REG_DEF(Build)
    SP_REG_DEF(LookAt, Vec3 const&)
    SP_REG_DEF(MoveTo, Vec3 const&)
    SP_REG_DEF(NavTo, Vec3 const&)
    SP_REG_DEF(Script, std::string const&, int, std::string const&)
};

} // namespace coral_fans::cfsp

BOOST_CLASS_VERSION(coral_fans::cfsp::SimPlayerManager, MANAGER_VERSION)
BOOST_CLASS_VERSION(coral_fans::cfsp::SimPlayerManager::SimPlayerInfo, INFO_VERSION)