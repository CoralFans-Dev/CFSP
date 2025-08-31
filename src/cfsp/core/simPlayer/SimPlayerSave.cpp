#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
#include "mc/nbt/CompoundTag.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <optional>

namespace coral_fans::cfsp::simulated_player {

void SimPlayer::save() {
    if (this->mShouldSave) {
        if (ll::config::saveConfig(
                this->mSaveData,
                CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.name / "data.json"
            ))
            this->mShouldSave = false;
    }
    if (!this->mSaveData.uniqueId.has_value()) {
        if (!this->mSimPlayer) return;
        auto tag = std::make_unique<CompoundTag>();
        if (!this->mSimPlayer->save(*tag)) return;
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

} // namespace coral_fans::cfsp::simulated_player