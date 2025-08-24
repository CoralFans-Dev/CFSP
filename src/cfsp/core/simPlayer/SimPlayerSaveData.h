#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/GameType.h"
#include <optional>
#include <string>


namespace coral_fans::cfsp::simulated_player {
struct SimPlayerSaveData {
    int                        version = 1;
    std::string                name;
    int64                      uniqueId;
    std::string                xuid;
    std::string                ownerUuid;
    bool                       isLockUniqueId;
    bool                       isOnline;
    GameType                   gameType;
    std::optional<std::string> lastSpawnerUuid; // 当由后台上线时，为std::nullopt_t
    Vec3                       lookAtOffSet;
    bool                       isEmptyInv;
};
} // namespace coral_fans::cfsp::simulated_player