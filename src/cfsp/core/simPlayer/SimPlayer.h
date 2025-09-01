#pragma once

#include "SimPlayerSaveData.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/OperateResult.h"
#include "mc/server/SimulatedPlayer.h"
#include <memory>

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
    bool                        loadSpNbt();
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline bool        isOnline();
    base::OperateResult         hasPermission(Player*, SimPlayerPermission permission);

public:
    CFSP_API void                              cancelTask();
    CFSP_API void                              cancelScript();
    CFSP_API void                              stopAction();
    CFSP_API void                              stop();
    CFSP_API static std::shared_ptr<SimPlayer> create(
        const Player*      player,
        std::string const& name,
        Vec3 const&        pos,
        DimensionType      dim,
        bool               isLockUniqueId = true
    );
    CFSP_API bool spawn(const Player*);
    CFSP_API void despawn();

public:
    CFSP_API bool isEmptyInv();
};
} // namespace coral_fans::cfsp::simulated_player