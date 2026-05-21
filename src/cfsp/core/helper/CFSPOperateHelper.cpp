#include "CFSPHelperManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "glm/fwd.hpp"
#include "ll/api/memory/Hook.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/world/level/block/Block.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#endif

namespace coral_fans::cfsp::helper {
LL_TYPE_INSTANCE_HOOK(
    CFSPBuildHelperHook,
    ll::memory::HookPriority::Normal,
    Block,
    &Block::use,
    bool,
    Player&                 player,
    ::BlockPos const&       pos,
    uchar                   face,
    ::std::optional<::Vec3> hit
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player, pos, face, hit);
#endif
    if (CFSPHelperManager::getInstance().buildMutex) return false;
    return origin(player, pos, face, hit);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPLookHelperHook,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::tick,
    bool,
    ::BlockSource& region
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(region);
#endif
    bool ori  = origin(region);
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value() && std::holds_alternative<::sim::VoidMoveIntent>(this->mSimulatedMovement->mType.get())) {
        auto pos = cfsp.value()->mSaveData.lookAtOffSet + this->getHeadPos();
        if (std::holds_alternative<::sim::ContinuousLookAtPositionIntent>(this->mLookAtIntent->mType.get()))
            [[likely]] {
            std::get<::sim::ContinuousLookAtPositionIntent>(this->mLookAtIntent->mType.get()).mPosition =
                glm::vec3(pos.x, pos.y, pos.z);
        } else cfsp.value()->lookAt(pos);
    }
    return ori;
}

void CFSPHelperManager::operateHelperHook(bool enabled) {
    if (enabled) {
        CFSPBuildHelperHook ::hook();
        CFSPLookHelperHook ::hook();
    } else {
        CFSPBuildHelperHook ::unhook();
        CFSPLookHelperHook ::unhook();
    }
}
} // namespace coral_fans::cfsp::helper