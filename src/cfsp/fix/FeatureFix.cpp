#include <mc/legacy/ActorRuntimeID.h>
#include <mc/network/NetworkBlockPosition.h>

#include "cfsp/CFSP.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/network/LoopbackPacketSender.h"
#include "mc/network/MinecraftPacketIds.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/PlayStatusPacket.h"
#include "mc/network/packet/PlayerActionPacket.h"
#include "mc/network/packet/PlayerAuthInputPacket.h"
#include "mc/network/packet/RequestChunkRadiusPacket.h"
#include "mc/network/packet/RespawnPacket.h"
#include "mc/network/packet/ShowCreditsPacket.h"
#include "mc/server/SimulatedPlayer.h"
#include "mc/world/actor/ActorInitializationMethod.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/chunk/ChunkViewSource.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/storage/DBStorage.h"

#include "ll/api/base/StdInt.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/thread/ServerThreadExecutor.h"

namespace coral_fans::cfsp::fix {
template <typename T>
void SimulateSendPacketToServer(SimulatedPlayer& sp, T& packet) {
    packet.mClientSubId = sp.getClientSubId();
    DEBUGW("({})::send - {}", sp.getNameTag(), packetToDebugString(packet));
    ll::service::getServerNetworkHandler()->handle(sp.getNetworkIdentifier(), packet);
}
template <typename T>
void SimulateSendPacketToServerAfter(SimulatedPlayer& sp, T&& packet, size_t delayTicks = 1) {
    packet.mSenderSubId = sp.getClientSubId();
    // DEBUGW("({})::send - {}", sp.getNameTag(), packetToDebugString(packet));
    ll::thread::ServerThreadExecutor::getDefault().executeAfter(
        [&nid = sp.getNetworkIdentifier(), packet = std::move(packet)]() {
            ll::service::getServerNetworkHandler()->handle(nid, packet);
        },
        ll::chrono::ticks{delayTicks}
    );
}


// ================= Fix chunk load =================
// fix chunk load and tick - ChunkSource load mode
LL_TYPE_INSTANCE_HOOK(
    LoadChunkFix_ChunkSource_LoadMode,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$_createChunkSource,
    ::std::shared_ptr<::ChunkViewSource>,
    ::ChunkSource& mainChunkSource
) {
    // if (FixManager::shouldFix(this)) {
    // auto cs = origin( mainChunkSource);
    // // ChunkSource::LoadMode : None(0) -> Deferred(1)
    // cs->mParentLoadMode = ::ChunkSource::LoadMode::Deferred;
    // return cs;
    return this->ServerPlayer::$_createChunkSource(mainChunkSource);
    // }
    // return origin(mainChunkSource);
}

// ================= Fix tick =================
// fix chunk load and tick - _updateChunkPublisherView
LL_TYPE_INSTANCE_HOOK(
    LoadChunkFix__updateChunkPublisherView,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$tickWorld,
    void,
    ::Tick const& tick
) {
    origin(tick);
    //  _updateChunkPublisherView will be called after Player::tick in ServerPlayer::tick
    // if (FixManager::shouldFix(this)) {
    // Force to call the implementation of ServerPlayer
    // seem this only for tell client which chunk ready in server
    this->ServerPlayer::$_updateChunkPublisherView(getPosition(), 16.0f);
    // }
}

// ================== Fix Travel ==================
// First change dimension from the end
LL_TYPE_INSTANCE_HOOK(
    TravelFix_ShowCredits,
    ll::memory::HookPriority::Normal,
    SimulatedPlayer,
    &SimulatedPlayer::$changeDimensionWithCredits,
    void,
    ::DimensionType dimension
) {
    origin(dimension);
    // if (FixManager::shouldFix(this)) {
    // mHasSeenCredits = true;
    ShowCreditsPacket packet{};
    packet.mPlayerID     = getRuntimeID();
    packet.mCreditsState = ShowCreditsPacket::CreditsState::Finished;
    SimulateSendPacketToServerAfter(*this, packet);
    // }
}

[[maybe_unused]] void
SimulateHandlePacketFromServer(SimulatedPlayer& sp, [[maybe_unused]] ShowCreditsPacket const& packet) {
    // DEBUGW("({})::handle - {}", sp.getNameTag(), packetToDebugString(packet));
    ShowCreditsPacket res;
    res.mPlayerID     = sp.getRuntimeID();
    res.mCreditsState = ShowCreditsPacket::CreditsState::Finished;
    SimulateSendPacketToServerAfter(sp, res, 1);
    return;
}
// fix state when changing dimension

// ================== Fix Initialized Spawn ==================
void SimulateHandlePacketFromServer(SimulatedPlayer& sp, [[maybe_unused]] PlayStatusPacket const& packet) {
    // SpawnFix chunk radius
    // DEBUGW("({})::handle - {}", sp.getNameTag(), packetToDebugString(packet));
    static int ChunkRadius = 5;

    RequestChunkRadiusPacket res;
    res.mChunkRadius    = ChunkRadius;
    res.mMaxChunkRadius = static_cast<uchar>(ChunkRadius);

    SimulateSendPacketToServerAfter(sp, res, 5);
}

bool SimulateHandlePacketFromServer(NetworkIdentifier const& nid, Packet const& packet, SubClientId subId) {
    auto sp = ll::service::getServerNetworkHandler()->_getServerPlayer(nid, subId);
    // if (!FixManager::shouldFix(sp)) return false;
    const auto packetId = packet.getId();
    switch (packetId) {
    // case MinecraftPacketIds::Respawn:
    //     SimulateHandlePacketFromServer(*static_cast<SimulatedPlayer*>(sp), static_cast<RespawnPacket
    //     const&>(packet)); break;
    case MinecraftPacketIds::ShowCredits:
        SimulateHandlePacketFromServer(
            *static_cast<SimulatedPlayer*>(sp),
            static_cast<ShowCreditsPacket const&>(packet)
        );
        break;
    case MinecraftPacketIds::PlayStatus:
        SimulateHandlePacketFromServer(
            *static_cast<SimulatedPlayer*>(sp),
            static_cast<PlayStatusPacket const&>(packet)
        );
        break;
    default:
        break;
    }
    return true;
}

// ================== Hook server outgoing packet ==================
LL_TYPE_INSTANCE_HOOK(
    FixByPacketHook,
    ll::memory::HookPriority::Normal,
    LoopbackPacketSender,
    &LoopbackPacketSender::$sendToClient,
    void,
    ::UserEntityIdentifierComponent const* userIdentifier,
    ::Packet const&                        packet
) {
    // TODO
    if (userIdentifier->mNetworkId == NetworkIdentifier::INVALID_ID()) {
        try {
            [[maybe_unused]] auto handled =
                SimulateHandlePacketFromServer(userIdentifier->mNetworkId, packet, userIdentifier->mClientSubId);
            // if (handled) return;
        } catch (...) {
            // lfp::LeviFakePlayer::getLogger().error(
            //     "Error in handle packet for SubClient<{}>",
            //     (int)userIdentifier->mClientSubId
            // );
        }
    }
    origin(std::forward<::UserEntityIdentifierComponent const*>(userIdentifier), std::forward<::Packet const&>(packet));
};


void featureFix(bool bl) {
    if (bl) {
        LoadChunkFix__updateChunkPublisherView::hook();
        LoadChunkFix_ChunkSource_LoadMode::hook();
        // RespawnFix_onPlayerDie::hook();
        TravelFix_ShowCredits::hook();
        // SpawnFix_Temp_initSpawnPos::hook();
        // SpawnFix_respawnPos::hook();
        FixByPacketHook::hook();
    } else {
        LoadChunkFix__updateChunkPublisherView::unhook();
        LoadChunkFix_ChunkSource_LoadMode::unhook();
        // RespawnFix_onPlayerDie::unhook();
        TravelFix_ShowCredits::unhook();
        // SpawnFix_Temp_initSpawnPos::hook();
        // SpawnFix_respawnPos::hook();
        FixByPacketHook::unhook();
    }
}
} // namespace coral_fans::cfsp::fix