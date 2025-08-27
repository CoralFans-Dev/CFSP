#pragma once

#include "cfsp/CommandPermission.h"
#include "cfsp/ConFig.h"
#include "cfsp/base/CommandResult.h"
#include "cfsp/core/group/SimPlayerGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/core/simPlayer/SimPlayerSaveData.h"
#include <memory>
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::manager {

class SimPlayerManager {
private:
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayerSaveData>>
        mOfflineSpDataMap; // <假人名, 不在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
                                                                            mOnlineSpMap; // <假人名, 在线的CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<group::SimPlayerGroup>> mGroupMap;    //<假人组名, 假人组对象>
    std::vector<std::string>                                                onlineList;
    std::vector<std::string>                                                offlineList;
    std::unordered_map<std::string, unsigned long long> mOnlineCountPerPlayer; //<玩家UUID, 上线假人数>
    unsigned long long                                  mOnlineCount;          // 总上线假人数
    std::shared_ptr<timewheel::TimeWheel>               mScheduler;
    config::Config                                      mConfig;
    struct config::CommandPermission                    mCommandPermission;

public:
    static SimPlayerManager&              getInstance();
    config::Config&                       getConfig();
    std::shared_ptr<timewheel::TimeWheel> getSchedule();
    bool                                  loadData();

private:
    void loadSpSaveData();
    void loadGroupData();
    bool isManager(Player* player);

public:
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(Player* sp);
    base::CommandResult
    createSp(Player* player, std::string const& name, Vec3 const& pos, DimensionType dim, bool isLockUniqueId = true);
};
} // namespace coral_fans::cfsp::manager