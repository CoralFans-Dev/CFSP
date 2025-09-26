#include "ll/api/event/ListenerBase.h"
#include "mc/world/level/storage/DBStorage.h"

namespace coral_fans::cfsp::helper {
class CFSPHelperManager {
private:
    ll::event::ListenerPtr playerJoinEventListener;

public:
    bool       buildMutex = false;
    DBStorage* dbStorage  = nullptr;

public:
    static CFSPHelperManager& getInstance();

private:
    void saveHelperHook();
    void autoFuncHelperRegister();
    void scheduleHelperHook();
    void operateHelperHook();
    void dbStorageHook();

public:
    void SimPlayerHelperHook();
    void SimPlayerHelperHookWhenLoad();
};
} // namespace coral_fans::cfsp::helper