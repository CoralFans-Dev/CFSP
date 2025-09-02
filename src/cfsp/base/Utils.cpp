#pragma once

#include "Utils.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"

namespace coral_fans::cfsp::utils {

std::string tryGetPlayerName(std::string UUID) {
    auto player = ll::service::getLevel()->getPlayer(mce::UUID(UUID));
    if (player) return *player->mName;
    else return UUID;
}

BlockPos neighbor(BlockPos pos, int face) {
    pos[(face / 2 + 1) % 3] += (face & 1) * 2 - 1;
    return pos;
}

} // namespace coral_fans::cfsp::utils