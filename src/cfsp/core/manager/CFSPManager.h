#pragma once

#include "TimeWheel.h"
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
    std::unordered_map<std::string, unsigned long long>                mOnlineCountPerPlayer; //<玩家UUID, 上线假人数>
    unsigned long long                                                 mOnlineCount;          // 总上线假人数
    std::shared_ptr<timewheel::TimeWheel>                              mScheduler;
    config::Config                                                     mConfig;
    struct config::PermissionConfig                                    mPermissionConfig;

public:
    static CFSPManager&                   getInstance();
    config::Config&                       getConfig();
    config::PermissionConfig&             getPermissionConfig();
    std::shared_ptr<timewheel::TimeWheel> getSchedule();
    bool                                  init();
    bool                                  isAllowed(Player*);
    bool                                  isManager(Player*);
    base::OperateResult                   canCreatePlayer(Player*);

public:
private:
    void loadSpSaveData();
    void loadGroupData();

public:
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(Player* sp);
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(std::string const& name);
    std::optional<std::shared_ptr<group::CFSPGroup>>            tryGetCFSPGroup(std::string const& name);
    std::vector<std::string>                                    getSpNamesSorted(const Player*);
    std::vector<std::string>                                    getGroupNamesSorted(const Player*);
    base::OperateResult
    createSp(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim, bool isLockUniqueId = true);
};
} // namespace coral_fans::cfsp::manager