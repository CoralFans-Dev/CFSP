#pragma once

#include "SimPlayerSaveData.h"
#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "mc/server/SimulatedPlayer.h"

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
public:
    SimPlayerSaveData                     mSaveData;
    std::unordered_set<std::string>       mGroups;
    SimulatedPlayer*                      mSimPlayer;
    std::shared_ptr<timewheel::TimeWheel> mScheduler;
    unsigned long long                    mTaskid;
    unsigned long long                    mScriptid;
    bool                                  mShouldSave;
    bool                                  mIsEnderContainerEmpty;
    bool                                  mIsInventoryEmpty;
    bool                                  mIsOffhandEmpty;
    bool                                  mIsEquipmentEmpty;

private:
    void simPlayerInvHook();

public:
    void                        save();
    void                        load();
    void                        simPlayerHook();
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline bool        isOnline();

public:
    CFSP_API bool isEmptyInv();
};
} // namespace coral_fans::cfsp::simulated_player