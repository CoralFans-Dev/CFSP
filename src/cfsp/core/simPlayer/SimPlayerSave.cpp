#include "SimPlayer.h"
#include "cfsp/CFSP.h"
#include "ll/api/Config.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"
#include "mc/deps/nbt/CompoundTag.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <filesystem>

namespace coral_fans::cfsp::simulated_player {

bool SimPlayer::save() {
#ifdef LL_PLAT_S
    auto dataDir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
#ifdef LL_PLAT_C
    auto worldDataDir = CFSP::getInstance().getSelf().getWorldDataDir();
    auto dataDir =
        worldDataDir ? worldDataDir.value() / "simplayer" : CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
    if (this->mShouldSave) {
        if (ll::config::saveConfig(
                this->mSaveData,
                dataDir / reinterpret_cast<const char8_t*>(this->mSaveData.name.c_str()) / "data.json"
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
        dataDir / reinterpret_cast<const char8_t*>(this->mSaveData.name.c_str()) / "nbt",
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
#ifdef LL_PLAT_S
    auto dataDir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
#ifdef LL_PLAT_C
    auto worldDataDir = CFSP::getInstance().getSelf().getWorldDataDir();
    auto dataDir =
        worldDataDir ? worldDataDir.value() / "simplayer" : CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
    auto path = dataDir / reinterpret_cast<const char8_t*>(this->mSaveData.name.c_str()) / "nbt";
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
            int dimId = it->second.get<IntTag>().data;
            this->tp(this->mSimPlayer->getFeetPos(), dimId);
        }
    } catch (const std::exception& e) {
        coral_fans::cfsp::CFSP::getInstance().getSelf().getLogger().error(e.what());
        return false;
    }
    return true;
}

bool SimPlayer::checkInvEmptyForOfflineCFSP() {
#ifdef LL_PLAT_S
    auto dataDir = CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
#ifdef LL_PLAT_C
    auto worldDataDir = CFSP::getInstance().getSelf().getWorldDataDir();
    auto dataDir =
        worldDataDir ? worldDataDir.value() / "simplayer" : CFSP::getInstance().getSelf().getDataDir() / "simplayer";
#endif
    auto path = dataDir / reinterpret_cast<const char8_t*>(this->mSaveData.name.c_str()) / "nbt";
    if (!std::filesystem::exists(path)) [[unlikely]]
        return false;
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) [[unlikely]]
        return false;
    try {
        boost::iostreams::mapped_file_source mmap;
        mmap.open(path.string());
        std::string_view                snbt{mmap.data(), mmap.size()};
        DefaultDataLoadHelper           helper;
        auto                            tag     = CompoundTag::fromSnbt(snbt).value();
        static std::vector<std::string> invKeys = {"Armor", "EnderChestInventory", "Inventory", "Offhand"};
        for (auto& key : invKeys) {
            if (auto it = tag.mTags.find(key); it != tag.mTags.end()) {
                for (auto& invTag : it->second.get<ListTag>()) {
                    auto& _invTag = invTag.get<CompoundTag>();
                    if (auto invIt = _invTag.mTags.find("Count"); invIt != _invTag.mTags.end()) {
                        if (invIt->second.get<ByteTag>().data) {
                            return false;
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        coral_fans::cfsp::CFSP::getInstance().getSelf().getLogger().error(e.what());
        return false;
    }
    return true;
}
} // namespace coral_fans::cfsp::simulated_player