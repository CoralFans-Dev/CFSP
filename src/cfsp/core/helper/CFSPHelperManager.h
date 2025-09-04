#include "ll/api/event/ListenerBase.h"

namespace coral_fans::cfsp::helper {
class CFSPHelperManager {
private:
    ll::event::ListenerPtr playerJoinEventListener;

public:
    bool buildMutex = false;

public:
    static CFSPHelperManager& getInstance();

private:
    void saveHelperHook();
    void autoFuncHelperRegister();
    void scheduleHelperHook();
    void operateHelperHook();

public:
    void SimPlayerHelperHook();
};
} // namespace coral_fans::cfsp::helper