#pragma once

#include "cfsp/ConFig.h"
#include "cfsp/PermissionConfig.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include <memory>
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::manager {

class CFSPManager {
private:
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
        mOfflineSpMap; // <假人名, 不在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
                                                                       mOnlineSpMap; // <假人名, 在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<group::CFSPGroup>> mGroupMap;    //<假人组名, 假人组对象>
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
    bool                      isAllowed(const Player*);
    bool                      isManager(const Player*);
    base::OperateResult       baseCheck(const Player*, config::FuncStruct);
    base::OperateResult       canCreatePlayer(const Player*);
    base::OperateResult       canSpawnPlayer(const Player*);
    base::OperateResult       canCreateGroup(const Player*);

private:
    bool tryCreateDiretory(const std::filesystem::path& basePath, const std::string& dir);
    void loadSpSaveData();
    void loadGroupData();

public:
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(Player* sp);
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(std::string const& name);
    std::optional<std::shared_ptr<group::CFSPGroup>>            tryGetCFSPGroup(std::string const& name);
    std::vector<std::string>                                    getSpNamesSorted(const Player*);
    std::vector<std::string>                                    getGroupNamesSorted(const Player*);
    std::vector<std::string>                                    getPublicSpNamesSorted();
    std::vector<std::string>                                    getAllSpNamesSorted();
    std::vector<std::string>                                    getAllGroupNamesSorted();
    std::string                                                 listOnlineSp(const Player*);
    std::string                                                 listOfflineSp(const Player*);

public:
    void                saveSps();
    base::OperateResult spCreate(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim);
    base::OperateResult createGroup(Player* player, std::string const& gname);
    base::OperateResult
    spSpawn(Player* player, std::string const& spname, bool nocheck = false, bool isLockUniqueId = true);
    base::OperateResult spDespawn(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spRespawn(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spRm(Player* player, std::string const& spname, bool nocheck = false, bool force = false);
    base::OperateResult spStop(Player* player, std::string const& spname, bool nocheck = false);

public:
    base::OperateResult spDrop(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spDropInv(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult spSwap(Player* player, std::string const& spname, bool nocheck = false);

public:
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

public:
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

public:
    base::OperateResult spChat(Player* player, std::string const& spname, std::string message, bool nocheck = false);
    base::OperateResult spRunCmd(Player* player, std::string const& spname, std::string cmd, bool nocheck = false);
};
} // namespace coral_fans::cfsp::manager