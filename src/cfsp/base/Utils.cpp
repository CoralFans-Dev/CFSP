#pragma once

#include "Utils.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/utils/StringUtils.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/ListTag.h"
#include "mc/world/Container.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/level/Level.h"
#include <string>


namespace coral_fans::cfsp::utils {

namespace {

struct PathNode {
    std::string id;
    int         index;
    bool        useIndex;
};

bool parsePath(std::string const& path, std::vector<PathNode>& vec) {
    for (auto key : ll::string_utils::splitByPattern(path, ".")) {
        if (key.ends_with(']')) try {
                vec.emplace_back(PathNode{
                    std::string{key.substr(0, key.find('['))},
                    std::stoi(std::string{key.substr(key.find('[') + 1, key.length() - key.find('[') - 2)}),
                    true
                });
            } catch (...) {
                return false;
            }
        else vec.emplace_back(PathNode{std::string{key}, 0, false});
    }
    return true;
}

} // namespace

std::pair<std::string, bool> getNbtFromTag(CompoundTag const tag, std::string const& path) {
    using ll::i18n_literals::operator""_tr;
    std::vector<PathNode>           nodes;
    std::vector<CompoundTagVariant> tags;
    if (!parsePath(path, nodes)) return {"translate.data.error.cannotparse"_tr(), false};
    try {
        tags.emplace_back(tag[nodes[0].id]);
        if (nodes[0].useIndex) {
            if (tags.back().is_array() && tags.back().get<ListTag>().getCompound(nodes[0].index))
                tags.emplace_back(*(tags.back().get<ListTag>().getCompound(nodes[0].index)));
            else return {"translate.data.error.notanarray"_tr(), false};
        }
        for (unsigned long long i = 1; i < nodes.size(); ++i) {
            tags.emplace_back(tags.back()[nodes[i].id]);
            if (nodes[i].useIndex) {
                if (tags.back().is_array() && tags.back().get<ListTag>().getCompound(nodes[i].index))
                    tags.emplace_back(*(tags.back().get<ListTag>().getCompound(nodes[i].index)));
                else return {"translate.data.error.notanarray"_tr(), false};
            }
        }
        return {tags.back().toSnbt(SnbtFormat::PrettyChatPrint), true};
    } catch (...) {
        return {"translate.data.error.geterror"_tr(), false};
    }
}

std::string removeMinecraftPrefix(std::string const& s) { return s.find("minecraft:") == 0 ? s.substr(10) : s; }

void swapItemInContainer(Player* player, int slot1, int slot2) {
    if (player) {
        auto&     container = *player->mInventory->mInventory;
        ItemStack i1, i2;
        i1 = (slot1 == -1) ? player->getOffhandSlot() : container.getItem(slot1);
        i2 = (slot2 == -1) ? player->getOffhandSlot() : container.getItem(slot2);
        if (slot1 == -1) player->setOffhandSlot(i2);
        else container.setItem(slot1, i2);
        if (slot2 == -1) player->setOffhandSlot(i1);
        else container.setItem(slot2, i1);
    }
}

std::string tryGetPlayerName(std::string UUID) {
    auto player = ll::service::getLevel()->getPlayer(mce::UUID(UUID));
    if (player) return *player->mName;
    else return UUID;
}

} // namespace coral_fans::cfsp::utils