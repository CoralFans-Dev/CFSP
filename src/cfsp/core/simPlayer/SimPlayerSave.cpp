#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"
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

bool SimPlayer::loadSpNbt() {
    if (!this->mSimPlayer) return false;
    auto path = cfsp::CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.name / "nbt";
    if (!std::filesystem::exists(path)) return false;
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) return false;
    try {
        boost::iostreams::mapped_file_source mmap;
        mmap.open(path.string());
        std::string_view      snbt{mmap.data(), mmap.size()};
        DefaultDataLoadHelper helper;
        this->mSimPlayer->load(CompoundTag::fromSnbt(snbt).value(), helper);
    } catch (const std::exception& e) {
        coral_fans::cfsp::CFSP::getInstance().getSelf().getLogger().error(e.what());
        return false;
    }
    return true;
}
} // namespace coral_fans::cfsp::simulated_player