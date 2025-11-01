#include "CFSPHelperManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/server/commands/StopCommand.h"
#include "mc/world/actor/provider/ActorEquipment.h"
#include "mc/world/level/storage/LevelStorage.h"

namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(
    CFSPSaveHelperHook,
    ::ll::memory::HookPriority::Normal,
    LevelStorage,
    &LevelStorage::save,
    void,
    ::Player& player
) {
    origin(player);
    if (auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(&player); cfsp.has_value()) cfsp.value()->save();
}

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

void CFSPHelperManager::saveHelperHook() {
    CFSPSaveHelperHook ::hook();
    CFSPSaveHelperHook2::hook();
}
} // namespace coral_fans::cfsp::helper