#pragma once

#include "SimPlayerSaveData.h"
#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "mc/server/SimulatedPlayer.h"

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
    SimPlayerSaveData                     mSaveData;
    SimulatedPlayer*                      mSimPlayer;
    std::shared_ptr<timewheel::TimeWheel> mScheduler;
    unsigned long long                    mTaskid;
    unsigned long long                    mScriptid;
    bool                                  mShouldSave;
    bool                                  mIsEnderContainerEmpty;
    bool                                  mIsInventoryEmpty;
    bool                                  mIsOffhandEmpty;
    bool                                  mIsEquipmentEmpty;

public:
    void                        save();
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline bool        isOnline();

public:
    CFSP_API bool isEmptyInv();
};
} // namespace coral_fans::cfsp::simulated_player