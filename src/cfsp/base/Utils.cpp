#pragma once

#include "Utils.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"

namespace coral_fans::cfsp::base::utils {

std::string tryGetPlayerName(std::string UUID) {
    auto player = ll::service::getLevel()->getPlayer(mce::UUID(UUID));
    if (player) return *player->mName;
    else return UUID;
}

BlockPos neighbor(BlockPos pos, int face) {
    pos[(face / 2 + 1) % 3] += (face & 1) * 2 - 1;
    return pos;
}

std::string getDimName(int dimId) {
    using ll::i18n_literals::operator""_tr;
    switch (dimId) {
    case 0:
        return "base.dimension.overworld"_tr();
    case 1:
        return "base.dimension.nether"_tr();
    case 2:
        return "base.dimension.theend"_tr();
    default:
        return "base.dimension.undefined"_tr();
    }
}

std::string getGameModeStr(int gameType) {
    using ll::i18n_literals::operator""_tr;
    switch (gameType) {
    case 0:
        return "base.gamemode.survival"_tr();
    case 1:
        return "base.gamemode.creative"_tr();
    case 2:
        return "base.gamemode.adventure"_tr();
    case 6:
        return "base.gamemode.spectator"_tr();
    default:
        return "base.gamemode.undefined"_tr();
    }
}
} // namespace coral_fans::cfsp::base::utils