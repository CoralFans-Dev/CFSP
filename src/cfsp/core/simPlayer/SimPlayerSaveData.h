#pragma once

#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include <optional>
#include <string>
#include <unordered_map>

namespace coral_fans::cfsp::simulated_player {
struct SimPlayerSaveData {
    int                                   version = 1;
    std::string                           name;
    std::string                           xuid;
    std::optional<int64>                  uniqueId = std::nullopt; // 不锁定uniqueId时，为std::nullopt
    std::string                           ownerUuid;               // 当由后台上线时，为""
    std::string                           lastSpawnerUuid;         // 当由后台上线时，为""
    uint                                  publicPermission = 0;
    std::unordered_map<std::string, uint> permission; // <玩家uuid, permission>
    bool                                  isOnline     = true;
    Vec3                                  lookAtOffSet = {0, 0, 0};
};
} // namespace coral_fans::cfsp::simulated_player