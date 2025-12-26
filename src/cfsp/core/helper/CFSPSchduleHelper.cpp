#include "CFSPHelperManager.h"
#include "cfsp/base/Schedule.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/Level.h"


namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(CFSPScheduleHelperHook, ll::memory::HookPriority::Normal, Level, &Level::$tick, void) {
    base::Schedule::getInstance().getSchedule()->tick();
    origin();
}

void CFSPHelperManager::scheduleHelperHook() { CFSPScheduleHelperHook ::hook(); }
} // namespace coral_fans::cfsp::helper