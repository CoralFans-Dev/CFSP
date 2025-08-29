#pragma once

#include "SimPlayerPermission.h"
#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include <optional>
#include <string>
#include <unordered_map>


namespace coral_fans::cfsp::simulated_player {
struct SimPlayerSaveData {
    int                                                  version = 1;
    std::string                                          name;
    int64                                                uniqueId;
    std::string                                          xuid;
    std::string                                          ownerUuid;
    bool                                                 isLockUniqueId;
    std::optional<std::string>                           lastSpawnerUuid; // 当由后台上线时，为std::nullopt_t
    std::unordered_map<std::string, SimPlayerPermission> permission; // <玩家uuid, permission>, 当uuid为""时，为公共假人
    bool                                                 isOnline     = true;
    Vec3                                                 lookAtOffSet = {0, 0, 0};
    bool                                                 isEmptyInv   = true;
};
} // namespace coral_fans::cfsp::simulated_player