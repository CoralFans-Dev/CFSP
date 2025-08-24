#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include <boost/iostreams/device/mapped_file.hpp>


namespace coral_fans::cfsp::simulated_player {

void SimPlayer::save() {
    if (this->shouldSave) {
        ll::config::saveConfig(
            this->mSaveData,
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.name / "data.json"
        );
    }
    if (!this->mSaveData.isLockUniqueId) {
        if (!this->simPlayer) return;
        auto tag = std::make_unique<CompoundTag>();
        if (!this->simPlayer->save(*tag)) return;
        if (!tag) return;
        std::ofstream f(
            CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.name / "nbt",
            std::ios_base::out | std::ios_base::trunc
        );
        if (!f.is_open()) return;
        f << tag->toSnbt(SnbtFormat::Minimize);
        f.close();
    }
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSimPlayerSaveHelperHook,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$setOffhandSlot,
    void,
    ItemStack const& item
) {
    if (isSimulatedPlayer()) {
        if () }
    origin(item);
}
} // namespace coral_fans::cfsp::simulated_player