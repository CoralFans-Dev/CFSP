#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec2.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/server/SimulatedPlayer.h"
#include "string"
#include "unordered_set"


namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
    std::string                           name;
    int64                                 uid;
    std::string                           xuid;
    std::string                           ownerUuid;
    std::string                           lastSpawner;
    std::unordered_set<std::string>       groups;
    int                                   status;
    Vec3                                  offlinePos;
    int                                   offlineDim;
    Vec2                                  offlineRot;
    std::string                           offlineGameType;
    bool                                  offlineEmptyInv;
    SimulatedPlayer*                      simPlayer;        // no-save
    std::shared_ptr<timewheel::TimeWheel> scheduler;        // no-save
    unsigned long long                    taskid;           // no-save
    unsigned long long                    scriptid;         // no-save
    std::vector<unsigned long long>       autoDespawnCount; // no-save
    unsigned long long                    autoDespawnI;     // no-save
    Vec3                                  lookatOffPos;     // no-save

public:
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline int         getStatus();
};
} // namespace coral_fans::cfsp::simulated_player