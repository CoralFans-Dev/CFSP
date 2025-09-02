#include "CFSPHelperManager.h"

namespace coral_fans::cfsp::helper {
CFSPHelperManager& CFSPHelperManager::getInstance() {
    static CFSPHelperManager instance;
    return instance;
}

void CFSPHelperManager::SimPlayerHelperHook() {
    saveHelperHook();
    autoFuncHelperRegister();
    scheduleHelperHook();
}
} // namespace coral_fans::cfsp::helper