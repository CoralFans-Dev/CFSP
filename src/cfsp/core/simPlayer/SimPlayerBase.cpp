#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
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

void SimPlayer::load() {}

inline std::string SimPlayer::getName() { return this->mSaveData.name; }

inline std::string SimPlayer::getXuid() { return this->mSaveData.xuid; }

inline bool SimPlayer::isOnline() { return this->simPlayer != nullptr; }
} // namespace coral_fans::cfsp::simulated_player