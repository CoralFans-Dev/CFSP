#pragma once

#include "SimPlayerSaveData.h"
#include "cfsp/base/Macros.h"
#include "mc/server/SimulatedPlayer.h"

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
    SimPlayerSaveData  mSaveData;
    SimulatedPlayer*   mSimPlayer;
    unsigned long long mTaskid     = 0;
    unsigned long long mScriptid   = 0;
    bool               mShouldSave = true;
    bool               mIsEnderContainerEmpty;
    bool               mIsInventoryEmpty;
    bool               mIsOffhandEmpty;
    bool               mIsEquipmentEmpty;

public:
    SimPlayer(SimPlayerSaveData saveData, SimulatedPlayer* sp = nullptr);

public:
    void                        save();
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline bool        isOnline();

public:
    CFSP_API bool isEmptyInv();
    bool          hasPermission(Player*);
};
} // namespace coral_fans::cfsp::simulated_player