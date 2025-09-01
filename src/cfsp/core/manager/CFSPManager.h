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
    config::Config&           getConfig();
    config::PermissionConfig& getPermissionConfig();
    bool                      getAutoRespawn();
    bool                      getAutoJoin();
    bool                      getAutoDespawn();
    void                      setAutoRespawn(bool);
    void                      setAutoJoin(bool);
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
    base::OperateResult
    createSp(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim, bool isLockUniqueId = true);
    base::OperateResult createGroup(Player* player, std::string const& gname);
    base::OperateResult spawnSp(Player* player, std::string const& spname, bool nocheck = false);
    base::OperateResult
    despawnSp(Player* player, std::string const& spname, bool nocheck = false, bool isAutoDespawn = false);
    base::OperateResult respawnSp(Player* player, std::string const& spname, bool nocheck = false);
};
} // namespace coral_fans::cfsp::manager