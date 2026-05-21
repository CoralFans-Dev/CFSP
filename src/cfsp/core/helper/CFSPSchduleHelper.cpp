#include "CFSPHelperManager.h"
#include "cfsp/base/Schedule.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/Level.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#endif


namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(CFSPScheduleHelperHook, ll::memory::HookPriority::Normal, Level, &Level::$tick, void) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin();
#endif
    base::Schedule::getInstance().getSchedule()->tick();
    origin();
}

void CFSPHelperManager::scheduleHelperHook(bool enabled) {
    if (enabled) {
        CFSPScheduleHelperHook ::hook();
    } else {
        CFSPScheduleHelperHook ::unhook();
    }
}
} // namespace coral_fans::cfsp::helper