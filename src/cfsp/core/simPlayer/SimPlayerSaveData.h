#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/GameType.h"
#include "string"
#include <optional>

namespace coral_fans::cfsp::simulated_player {
struct SimPlayerSaveData {
    std::string                name;
    int64                      uniqueId;
    std::string                xuid;
    std::string                ownerUuid;
    std::optional<std::string> lastSpawnerUuid; // 当由后台上线时，为std::nullopt_t
    int                        status;
    Vec3                       offlinePos;
    int                        offlineDim;
    Vec3                       offlineLookAtOffSet;
    GameType                   offlineGameType;
    bool                       offlineEmptyInv;
};
} // namespace coral_fans::cfsp::simulated_player