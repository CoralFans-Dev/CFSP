#include "SimPlayerHelper.h"

namespace coral_fans::cfsp::helper {
SimPlayerHelperManager& SimPlayerHelperManager::getInstance() {
    static SimPlayerHelperManager instance;
    return instance;
}

void SimPlayerHelperManager::SimPlayerHelperHook() { SimPlayerSaveHook(); }
} // namespace coral_fans::cfsp::helper