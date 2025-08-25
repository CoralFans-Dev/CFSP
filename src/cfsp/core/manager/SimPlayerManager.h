#pragma once

#include "cfsp/ConFig.h"
#include "cfsp/core/group/SimPlayerGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "mc/server/SimulatedPlayer.h"
#include <memory>
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::manager {

class SimPlayerManager {
private:
    std::unordered_map<std::string, std::shared_ptr<simulated_player::SimPlayer>>
                                                                            mNameSimPlayerMap; // <假人名, CF假人对象>
    std::unordered_map<std::string, std::shared_ptr<group::SimPlayerGroup>> mGroupMap;         //<假人组名, 假人组对象>
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<simulated_player::SimPlayer>>>
                                          mOwnerSimPlayerMap; //<玩家UUID, CF假人对象>
    std::vector<std::string>              onlineList;
    std::vector<std::string>              offlineList;
    std::unordered_map<std::string, int>  mOnlineCountPerPlayer; //<玩家UUID, 上线假人数>
    int                                   mOnlineCount;          // 总上线假人数
    std::shared_ptr<timewheel::TimeWheel> mScheduler;
    config::Config                        mConfig;

public:
    static SimPlayerManager& getInstance();

public:
    config::Config& getConfig();

public:
    std::optional<std::shared_ptr<simulated_player::SimPlayer>> tryGetCFSP(SimulatedPlayer* sp);
};
} // namespace coral_fans::cfsp::manager