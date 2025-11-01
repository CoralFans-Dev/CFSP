#include "CFSPFixManager.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "mc/network/LoopbackPacketSender.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/PlayStatusPacket.h"
#include "mc/network/packet/RequestChunkRadiusPacket.h"
#include "mc/network/packet/ShowCreditsPacket.h"
#include "mc/server/SimulatedPlayer.h"

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
    return this->ServerPlayer::$_createChunkSource(mainChunkSource);
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
    this->ServerPlayer::$_updateChunkPublisherView(getPosition(), 16.0f);
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
    ShowCreditsPacket packet{};
    packet.mPlayerID     = getRuntimeID();
    packet.mCreditsState = ShowCreditsPacket::CreditsState::Finished;
    SimulateSendPacketToServerAfter(*this, packet);
}

[[maybe_unused]] void
SimulateHandlePacketFromServer(SimulatedPlayer& sp, [[maybe_unused]] ShowCreditsPacket const& packet) {
    ShowCreditsPacket res;
    res.mPlayerID     = sp.getRuntimeID();
    res.mCreditsState = ShowCreditsPacket::CreditsState::Finished;
    SimulateSendPacketToServerAfter(sp, res, 1);
    return;
}
// fix state when changing dimension

// ================== Fix Initialized Spawn ==================
void SimulateHandlePacketFromServer(SimulatedPlayer& sp, [[maybe_unused]] PlayStatusPacket const& packet) {
    RequestChunkRadiusPacket res;
    res.mChunkRadius    = sp.mChunkRadius;
    res.mMaxChunkRadius = static_cast<uchar>(sp.mChunkRadius);

    SimulateSendPacketToServerAfter(sp, res, 5);
}

bool SimulateHandlePacketFromServer(NetworkIdentifier const& nid, Packet const& packet, SubClientId subId) {
    auto       sp       = ll::service::getServerNetworkHandler()->_getServerPlayer(nid, subId);
    const auto packetId = packet.getId();
    switch (packetId) {
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
    if (userIdentifier->mNetworkId == NetworkIdentifier::INVALID_ID()) {
        try {
            [[maybe_unused]] auto handled =
                SimulateHandlePacketFromServer(userIdentifier->mNetworkId, packet, userIdentifier->mClientSubId);
        } catch (...) {}
    }
    origin(std::forward<::UserEntityIdentifierComponent const*>(userIdentifier), std::forward<::Packet const&>(packet));
};

void CFSPFixManager::featureFix() {
    LoadChunkFix__updateChunkPublisherView::hook();
    LoadChunkFix_ChunkSource_LoadMode::hook();
    TravelFix_ShowCredits::hook();
    FixByPacketHook::hook();
}
} // namespace coral_fans::cfsp::fix