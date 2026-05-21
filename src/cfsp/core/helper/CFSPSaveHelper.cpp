#include "CFSPHelperManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include "mc/world/level/storage/LevelStorage.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#endif


#ifdef LL_PLAT_S
#include "cfsp/CFSP.h"
#include "mc/server/commands/StopCommand.h"
#endif

namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook,
    ::ll::memory::HookPriority::Normal,
    LevelStorage,
    &LevelStorage::save,
    void,
    ::Player& player
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player);
#endif
    origin(player);
    if (auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(&player); cfsp.has_value()) cfsp.value()->save();
}

#ifdef LL_PLAT_S
LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook2,
    ll::memory::HookPriority::Normal,
    StopCommand,
    &StopCommand::$execute,
    void,
    CommandOrigin const& arg1,
    CommandOutput&       arg2
) {
    manager::CFSPManager::getInstance().saveSps();
    origin(arg1, arg2);
}
#endif

void CFSPHelperManager::saveHelperHook(bool enabled) {
    if (enabled) {
        CFSPSaveHelperHook ::hook();
#ifdef LL_PLAT_S
        CFSPSaveHelperHook2::hook();
#endif
    } else {
        CFSPSaveHelperHook ::unhook();
#ifdef LL_PLAT_S
        CFSPSaveHelperHook2::unhook();
#endif
    }
}
} // namespace coral_fans::cfsp::helper