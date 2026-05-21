#include "CFSPHelperManager.h"
#include "cfsp/base/Schedule.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"
#include <queue>
#include <unordered_map>
#include <vector>

#ifdef LL_PLAT_C
#include "mc/server/ServerInstance.h"
#endif


namespace coral_fans::cfsp::helper {
bool shouldDespawn(const std::string& spname) {
    if (!manager::CFSPManager::getInstance().getAutoDespawn()) return false;
    static std::unordered_map<std::string, std::queue<unsigned long long>> recordData;
    auto&                                                                  queue = recordData[spname];
    auto currentTime         = ll::service::getLevel()->getCurrentTick().tickID;
    auto autoDespawninterval = manager::CFSPManager::getInstance().getConfig().autoDespawninterval;
    queue.push(currentTime);
    std::erase_if(recordData, [currentTime, autoDespawninterval](auto& q) {
        while (currentTime - q.second.front() > autoDespawninterval) {
            q.second.pop();
            if (!q.second.size()) return true;
        }
        return false;
    });
    if (auto it = recordData.find(spname);
        it->second.size() >= manager::CFSPManager::getInstance().getConfig().autoDespawnCount) {
        recordData.erase(it);
        return true;
    }
    return false;
}

LL_TYPE_INSTANCE_HOOK(
    CFSPAutoFuncHelperHook,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$die,
    void,
    ActorDamageSource const& source
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(source);
#endif
    origin(source);
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value()) {
        cfsp.value()->stop();
        ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues(
            "cfspDeadSp",
            {cfsp.value()->mSaveData.name}
        );
        if (manager::CFSPManager::getInstance().getAutoRespawn()) {
            cfsp.value()->mTaskid =
                base::Schedule::getInstance().getSchedule()->add(20, [cfsp = cfsp.value()](unsigned long long) {
                    if (!cfsp->respawn()) return false;
                    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues(
                        "cfspDeadSp",
                        {cfsp->mSaveData.name}
                    );
                    if (shouldDespawn(cfsp->getName())) manager::CFSPManager::getInstance().autoDespawn(cfsp);
                    return false;
                });
        }
    }
}

void CFSPHelperManager::autoFuncHelperRegister(bool enabled) {
    enabled ? CFSPAutoFuncHelperHook::hook() : CFSPAutoFuncHelperHook::unhook();
}
} // namespace coral_fans::cfsp::helper