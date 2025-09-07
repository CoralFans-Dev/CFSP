#pragma once

#include "SimPlayerPermission.h"
#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::simulated_player {
struct SimPlayerSaveData {
    int                                                  version = 1;
    std::string                                          name;
    std::string                                          xuid;
    int64                                                uniqueId;        // 不锁定uniqueId时，为std::nullopt
    std::string                                          ownerUuid;       // 当由后台上线时，为""
    std::string                                          lastSpawnerUuid; // 当由后台上线时，为""
    SimPlayerPermission                                  publicPermission;
    std::unordered_map<std::string, SimPlayerPermission> permission; // <玩家uuid, permission>
    bool                                                 isOnline     = true;
    Vec3                                                 lookAtOffSet = {0, 0, 0};
    bool                                                 isEmptyInv   = true;
};
} // namespace coral_fans::cfsp::simulated_player