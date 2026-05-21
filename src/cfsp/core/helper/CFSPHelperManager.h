#pragma once

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
    void saveHelperHook(bool);
    void scheduleHelperHook(bool);
    void operateHelperHook(bool);
    void autoFuncHelperRegister(bool);

public:
    void SimPlayerHelperHook(bool);
};
} // namespace coral_fans::cfsp::helper