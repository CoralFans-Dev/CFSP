#pragma once

#include "cfsp/ConFig.h"
#include "cfsp/PermissionConfig.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::manager {

class CFSPManager {
private:
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
        mOfflineSpMap; // <假人名, 不在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
        mOnlineSpMap; // <假人名, 在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<group::CFSPGroup>> mGroupMap; //<假人组名, 假人组对象>
    config::Config                                                     mConfig;
    config::PermissionConfig                                           mPermissionConfig;

public:
    static CFSPManager&       getInstance();
    void                      save();
    void                      autoJoin();
    void                      autoDespawn(std::shared_ptr<simulated_player::SimPlayer> cfsp);
    config::Config&           getConfig();
    config::PermissionConfig& getPermissionConfig();
    bool                      getAutoJoin();
    bool                      getAutoRespawn();
    bool                      getAutoDespawn();
    void                      setAutoJoin(bool);
    void                      setAutoRespawn(bool);
    void                      setAutoDespawn(bool);
    bool                      init();
    void                      load();

public:
    bool                isAllowed(const Player*);
    bool                isManager(const Player*);
    base::OperateResult baseCheck(const Player*, config::FuncStruct);
    base::OperateResult canCreatePlayer(const Player*);
    base::OperateResult canSpawnPlayer(const Player*);
    base::OperateResult canCreateGroup(const Player*);
    uint                getSpPermissionMask(std::optional<CommandPermissionLevel> level = std::nullopt);
    uint                getGroupPermissionMask(std::optional<CommandPermissionLevel> level = std::nullopt);
    base::OperateResult spPerm(
        const Player*              player,
        std::string                spname,
        uint                       perm,
        bool                       enable,
        std::optional<std::string> targetUUid = std::nullopt
    );
    base::OperateResult groupPerm(
        const Player*              player,
        std::string                gname,
        uint                       perm,
        bool                       enable,
        std::optional<std::string> targetUUid = std::nullopt
    );

private:
    bool tryCreateDiretory(const std::filesystem::path& basePath, const std::string& dir);
    void loadSpSaveData();
    void loadGroupData();
    uint getSpBasePermissionMask();
    uint getSpPermissionMask(CommandPermissionLevel);
    uint getGroupBasePermissionMask();
    uint getGroupPermissionMask(CommandPermissionLevel);

public:
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(Actor* sp);
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(Player* sp);
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(std::string const& name);
    std::optional<std::shared_ptr<group::CFSPGroup>>            tryGetCFSPGroup(std::string const& name);
    std::vector<std::string>                                    getSpNamesSorted(const Player*);
    std::vector<std::string>                                    getGroupNamesSorted(const Player*);
    std::vector<std::string>                                    getPublicSpNamesSorted();
    std::vector<std::string>                                    getAllSpNamesSorted();
    std::vector<std::string>                                    getAllGroupNamesSorted();
    std::vector<std::string>                                    getCanBeAddedSpList(const Player*);
    std::string                                                 listOnlineSp(const Player*);
    std::string                                                 listOfflineSp(const Player*);
    std::string                                                 listGroup(const Player*);

public:
    void saveSps();
    base::OperateResult
    spCreate(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim, bool lockUniqueId = true);
    base::OperateResult spSpawn(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spDespawn(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spRespawn(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spDelete(Player* player, std::string const& spname, bool force = false, bool nocheck = false);
    base::OperateResult spStop(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spInfo(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spLookAt(Player* player, std::string const& spname, Vec3 const& pos, bool nocheck = false);

    base::OperateResult spInvInfo(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult
    spDrop(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult
    spDropInv(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult spSwap(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spSelect(Player* player, std::string const& spname, int id, bool nocheck = false);

    base::OperateResult spSneaking(
        Player*             player,
        std::string const&  spname,
        std::optional<bool> enabled = std::nullopt,
        bool                nocheck = false
    );
    base::OperateResult spSwimming(
        Player*             player,
        std::string const&  spname,
        std::optional<bool> enabled = std::nullopt,
        bool                nocheck = false
    );
    base::OperateResult spFlying(
        Player*             player,
        std::string const&  spname,
        std::optional<bool> enabled = std::nullopt,
        bool                nocheck = false
    );
    base::OperateResult spSprinting(
        Player*             player,
        std::string const&  spname,
        std::optional<bool> enabled = std::nullopt,
        bool                nocheck = false
    );

    base::OperateResult
    spAttack(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult
    spBuild(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult
    spInteract(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult
    spJump(Player* player, std::string const& spname, int times = 1, int interval = 1, bool nocheck = false);
    base::OperateResult spUse(
        Player*            player,
        std::string const& spname,
        int                _long    = 10,
        int                times    = 1,
        int                interval = 1,
        bool               nocheck  = false
    );
    base::OperateResult spDestroy(
        Player*            player,
        std::string const& spname,
        int                _long    = 1,
        int                times    = 1,
        int                interval = 1,
        bool               nocheck  = false
    );

    base::OperateResult
    spChat(Player* player, std::string const& spname, std::string const& message, bool nocheck = false);
    base::OperateResult
    spRunCmd(Player* player, std::string const& spname, std::string const& cmd, bool nocheck = false);

    base::OperateResult
    spMoveTo(Player* player, std::string const& spname, Vec3 const& pos, float speed = 4.3f, bool nocheck = false);
    base::OperateResult
    spNavTo(Player* player, std::string const& spname, Vec3 const& pos, float speed = 4.3f, bool nocheck = false);
    base::OperateResult spTp(
        Player*            player,
        std::string const& spname,
        Vec3               pos,
        std::optional<int> dimId   = std::nullopt,
        bool               nocheck = false
    );

public:
    base::OperateResult              groupCreate(Player* player, std::string const& gname);
    base::OperateResult              groupDelete(Player* player, std::string const& gname);
    base::OperateResult              groupAddSp(Player* player, std::string const& gname, std::string const& spname);
    base::OperateResult              groupRmSp(Player* player, std::string const& gname, std::string const& spname);
    std::vector<base::OperateResult> groupInfo(Player* player, std::string const& gname);
    std::vector<base::OperateResult> groupInvInfo(Player* player, std::string const& gname);

    std::vector<base::OperateResult> groupSpawn(Player* player, std::string const& gname);
    std::vector<base::OperateResult> groupDespawn(Player* player, std::string const& gname);
    std::vector<base::OperateResult> groupRespawn(Player* player, std::string const& gname);
    std::vector<base::OperateResult> groupDeleteSp(Player* player, std::string const& gname, bool force = false);

    std::vector<base::OperateResult> groupStop(Player* player, std::string const& gname);

    std::vector<base::OperateResult>
    groupDrop(Player* player, std::string const& gname, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupDropInv(Player* player, std::string const& gname, int times = 1, int interval = 1);

    std::vector<base::OperateResult>
    groupSneaking(Player* player, std::string const& gname, std::optional<bool> enabled = std::nullopt);
    std::vector<base::OperateResult>
    groupSwimming(Player* player, std::string const& gname, std::optional<bool> enabled = std::nullopt);
    std::vector<base::OperateResult>
    groupFlying(Player* player, std::string const& gname, std::optional<bool> enabled = std::nullopt);
    std::vector<base::OperateResult>
    groupSprinting(Player* player, std::string const& gname, std::optional<bool> enabled = std::nullopt);

    std::vector<base::OperateResult>
    groupAttack(Player* player, std::string const& gname, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupBuild(Player* player, std::string const& gname, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupInteract(Player* player, std::string const& gname, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupJump(Player* player, std::string const& gname, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupUse(Player* player, std::string const& gname, int _long = 10, int times = 1, int interval = 1);
    std::vector<base::OperateResult>
    groupDestroy(Player* player, std::string const& gname, int _long = 1, int times = 1, int interval = 1);

    std::vector<base::OperateResult> groupChat(Player* player, std::string const& gname, std::string const& message);
    std::vector<base::OperateResult> groupRunCmd(Player* player, std::string const& gname, std::string const& message);

    std::vector<base::OperateResult> groupLookAt(Player* player, std::string const& gname, Vec3 const& pos);

    std::vector<base::OperateResult>
    groupMoveTo(Player* player, std::string const& gname, Vec3 const& pos, float speed = 4.3f);
    std::vector<base::OperateResult>
    groupNavTo(Player* player, std::string const& gname, Vec3 const& pos, float speed = 4.3f);
    std::vector<base::OperateResult>
    groupTp(Player* player, std::string const& gname, Vec3 pos, std::optional<int> dimId = std::nullopt);

    std::vector<base::OperateResult> groupSelect(Player* player, std::string const& spname, int id);
};
} // namespace coral_fans::cfsp::manager