#include "SimPlayerSaveData.h"
#include "TimeWheel.h"
#include "cfsp/base/Macros.h"
#include "mc/server/SimulatedPlayer.h"

namespace coral_fans::cfsp::simulated_player {
class SimPlayer {
public:
public:
    SimPlayerSaveData                     mSaveData;
    std::unordered_set<std::string>       groups;
    SimulatedPlayer*                      simPlayer;
    std::shared_ptr<timewheel::TimeWheel> scheduler;
    unsigned long long                    taskid;
    unsigned long long                    scriptid;
    bool                                  shouldSave;

public:
    void                        save();
    void                        load();
    CFSP_API inline std::string getName();
    CFSP_API inline std::string getXuid();
    CFSP_API inline bool        isOnline();

public:
    CFSP_API bool isEmptyInv();
};
} // namespace coral_fans::cfsp::simulated_player