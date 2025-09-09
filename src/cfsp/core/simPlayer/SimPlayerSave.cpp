#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"
#include "mc/nbt/CompoundTag.h"
#include <boost/iostreams/device/mapped_file.hpp>


namespace coral_fans::cfsp::simulated_player {

bool SimPlayer::save() {
    if (this->mShouldSave) {
        if (ll::config::saveConfig(
                this->mSaveData,
                CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.xuid / "data.json"
            )) [[likely]]
            this->mShouldSave = false;
    }
    if (!this->mSimPlayer) [[unlikely]]
        return false;
    auto tag = std::make_unique<CompoundTag>();
    if (!this->mSimPlayer->save(*tag)) [[unlikely]]
        return false;
    if (!tag) [[unlikely]]
        return false;
    std::ofstream f(
        CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.xuid / "nbt",
        std::ios_base::out | std::ios_base::trunc
    );
    if (!f.is_open()) [[unlikely]]
        return false;
    f << tag->toSnbt(SnbtFormat::Minimize);
    f.close();
    return true;
}

bool SimPlayer::loadSpNbt() {
    if (!this->mSimPlayer) [[unlikely]]
        return false;
    auto path = cfsp::CFSP::getInstance().getSelf().getDataDir() / "simplayer" / this->mSaveData.xuid / "nbt";
    if (!std::filesystem::exists(path)) [[unlikely]]
        return false;
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) [[unlikely]]
        return false;
    try {
        boost::iostreams::mapped_file_source mmap;
        mmap.open(path.string());
        std::string_view      snbt{mmap.data(), mmap.size()};
        DefaultDataLoadHelper helper;
        auto                  tag = CompoundTag::fromSnbt(snbt).value();
        this->mSimPlayer->load(tag, helper);
        if (auto it = tag.mTags.find("DimensionId"); it != tag.mTags.end()) {
            int dimId = it->second;
            this->tp(this->mSimPlayer->getFeetPos(), dimId);
        }
    } catch (const std::exception& e) {
        coral_fans::cfsp::CFSP::getInstance().getSelf().getLogger().error(e.what());
        return false;
    }
    return true;
}
} // namespace coral_fans::cfsp::simulated_player