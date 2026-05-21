#include "CFSPHelperManager.h"

namespace coral_fans::cfsp::helper {
CFSPHelperManager& CFSPHelperManager::getInstance() {
    static CFSPHelperManager instance;
    return instance;
}

void CFSPHelperManager::SimPlayerHelperHook(bool enabled) {
    saveHelperHook(enabled);
    autoFuncHelperRegister(enabled);
    scheduleHelperHook(enabled);
    operateHelperHook(enabled);
}
} // namespace coral_fans::cfsp::helper