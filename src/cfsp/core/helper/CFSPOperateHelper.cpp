#include "CFSPHelperManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "glm/fwd.hpp"
#include "ll/api/memory/Hook.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/world/level/block/Block.h"

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
    bool ori  = origin(region);
    auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(this);
    if (cfsp.has_value() && std::holds_alternative<::sim::VoidMoveIntent>(this->mSimulatedMovement->mType.get())) {
        auto pos = cfsp.value()->mSaveData.lookAtOffSet + this->getEyePos();
        if (std::holds_alternative<::sim::ContinuousLookAtPositionIntent>(this->mLookAtIntent->mType.get()))
            [[likely]] {
            std::get<::sim::ContinuousLookAtPositionIntent>(this->mLookAtIntent->mType.get()).mPosition =
                glm::vec3(pos.x, pos.y, pos.z);
        } else cfsp.value()->lookAt(pos);
    }
    return ori;
}

void CFSPHelperManager::operateHelperHook() {
    CFSPBuildHelperHook ::hook();
    CFSPLookHelperHook ::hook();
}
} // namespace coral_fans::cfsp::helper