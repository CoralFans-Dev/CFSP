#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/server/SimulatedPlayer.h"

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
    enum SimPlayerStatus : int {
        Offline = 0,
        Alive   = 1,
        Dead    = 2,
    };

public:
    std::string                           name;
    std::string                           xuid;
    SimPlayerStatus                       status;
    std::unordered_set<std::string>       groups;
    SimulatedPlayer*                      simPlayer;        // no-save
    std::shared_ptr<timewheel::TimeWheel> scheduler;        // no-save
    unsigned long long                    taskid;           // no-save
    unsigned long long                    scriptid;         // no-save
    std::vector<unsigned long long>       autoDespawnCount; // no-save
    unsigned long long                    autoDespawnI;     // no-save
    Vec3                                  lookatOffPos;     // no-save

public:
    void                            save();
    CFSP_API inline std::string     getName();
    CFSP_API inline std::string     getXuid();
    CFSP_API inline SimPlayerStatus getStatus();
};
} // namespace coral_fans::cfsp::simulated_player