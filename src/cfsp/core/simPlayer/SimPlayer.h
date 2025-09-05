#pragma once

#include "SimPlayerSaveData.h"
#include "cfsp/base/Macros.h"
#include "cfsp/base/OperateResult.h"
#include "mc/server/SimulatedPlayer.h"
#include <memory>
#include <optional>

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
    SimPlayerSaveData  mSaveData;
    SimulatedPlayer*   mSimPlayer             = nullptr;
    unsigned long long mTaskid                = 0;
    unsigned long long mScriptid              = 0;
    bool               mShouldSave            = true;
    bool               mIsEnderContainerEmpty = true;
    bool               mIsInventoryEmpty      = true;
    bool               mIsOffhandEmpty        = true;
    bool               mIsEquipmentEmpty      = true;

public:
    SimPlayer(SimPlayerSaveData saveData, SimulatedPlayer* sp = nullptr);

public:
    bool     save();
    bool     loadSpNbt();
    CFSP_API std::string getName();
    CFSP_API std::string getXuid();
    CFSP_API bool        isOnline();
    CFSP_API bool        isFree();
    base::OperateResult  hasPermission(Player*, SimPlayerPermission permission);

public:
    CFSP_API void cancelTask();
    CFSP_API void cancelScript();
    CFSP_API base::OperateResult stop();
    CFSP_API static std::shared_ptr<SimPlayer>
             create(const Player* player, std::string const& name, Vec3 const& pos, DimensionType dim);
    CFSP_API base::OperateResult
    spawn(std::optional<const Player*> player, bool lockUniqueId = true); // 当player为std::nullopt时，为autojoin
    CFSP_API base::OperateResult despawn();
    CFSP_API base::OperateResult respawn();
    CFSP_API base::OperateResult lookAt(Vec3 const& pos);

public:
    CFSP_API bool isEmptyInv();
    CFSP_API base::OperateResult drop();
    CFSP_API base::OperateResult dropInv();
    CFSP_API base::OperateResult swap(Player*);

public:
    CFSP_API base::OperateResult sneaking(std::optional<bool> enable = std::nullopt);
    CFSP_API base::OperateResult swimming(std::optional<bool> enable = std::nullopt);
    CFSP_API base::OperateResult flying(std::optional<bool> enable = std::nullopt);
    CFSP_API base::OperateResult sprinting(std::optional<bool> enable = std::nullopt);

public:
    CFSP_API base::OperateResult attack(int times = 1, int interval = 1);
    CFSP_API base::OperateResult build(int times = 1, int interval = 1);
    CFSP_API base::OperateResult interact(int times = 1, int interval = 1);
    CFSP_API base::OperateResult jump(int times = 1, int interval = 1);
    CFSP_API base::OperateResult use(int _long = 10, int times = 1, int interval = 1);
    CFSP_API base::OperateResult destroy(int _long = 1, int times = 1, int interval = 1);

public:
    CFSP_API base::OperateResult chat(std::string&);
    CFSP_API base::OperateResult runcmd(std::string&);
};
} // namespace coral_fans::cfsp::simulated_player