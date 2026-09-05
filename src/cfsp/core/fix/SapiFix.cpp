#include "CFSPFixManager.h"
// #include "cfsp/CFSP.h"
#include "cfsp/core/manager/CFSPManager.h"
// #include "entt/meta/meta.hpp"
#include "ll/api/memory/Hook.h"
// #include "ll/api/service/Bedrock.h"
#include "mc/deps/ecs./WeakEntityRef.h"
// #include "mc/deps/scripting/runtime/BaseError.h"
// #include "mc/deps/scripting/runtime/CallStackEntry.h"
// #include "mc/deps/scripting/runtime/InvalidArgumentError.h"
// #include "mc/deps/scripting/runtime/Result.h"
// #include "mc/deps/scripting/runtime/ResultAny.h"
#include "mc/scripting/modules/minecraft/actor/ScriptActorFactory.h"
#include "mc/scripting/modules/minecraft/actor/ScriptActorQuery.h"
// #include "mc/scripting/modules/minecraft/commands/ScriptCommandError.h"
#include "mc/scripting/modules/minecraft/events/ScriptActorEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptActorGlobalEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptBlockGlobalEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptLevelGlobalEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptPlayerEventListener.h"
#include "mc/scripting/modules/minecraft/player/ScriptPlayer.h"
#include "mc/server/ServerLevel.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/events/ActorAcquiredItemEvent.h"
#include "mc/world/events/ActorAddEffectEvent.h"
#include "mc/world/events/ActorAttackEvent.h"
#include "mc/world/events/ActorDefinitionEndedEvent.h"
#include "mc/world/events/ActorDiedEvent.h"
#include "mc/world/events/ActorDroppedItemEvent.h"
#include "mc/world/events/ActorHealEvent.h"
#include "mc/world/events/ActorHealthChangedEvent.h"
#include "mc/world/events/ActorHurtEvent.h"
#include "mc/world/events/ActorRemovedEvent.h"
#include "mc/world/events/EventResult.h"
#include "mc/world/events/PlayerDimensionChangeAfterEvent.h"
#include "mc/world/events/PlayerEmoteEvent.h"
#include "mc/world/events/PlayerGameModeChangeEvent.h"
#include "mc/world/events/PlayerHotbarSelectedSlotChangeEvent.h"
#include "mc/world/events/PlayerInitialSpawnEvent.h"
#include "mc/world/events/PlayerInputModeChangeEvent.h"
#include "mc/world/events/PlayerInputPermissionCategoryChangeEvent.h"
#include "mc/world/events/PlayerInteractWithBlockAfterEvent.h"
#include "mc/world/events/PlayerInteractWithEntityAfterEvent.h"
#include "mc/world/events/PlayerInventoryItemChangeEvent.h"
#include "mc/world/events/PlayerRespawnEvent.h"
#include "mc/world/events/PlayerScriptInputEvent.h"
#include "mc/world/events/PlayerSwingStartEvent.h"
#include "mc/world/events/PlayerUseNameTagEvent.h"


// #include "mc/world/events/ProjectileHitEvent.h"
#include "mc/world/level/Level.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#endif
// #include <cstddef>
// #include <vector>


// Scripting::InvalidArgumentError::InvalidArgumentError() = default;
// // ScriptModuleMinecraft::ScriptPlayer::ScriptPlayer()     = default;
// Scripting::BaseError::BaseError()                                                                          = default;
// ScriptModuleMinecraft::ScriptPlayer& ScriptModuleMinecraft::ScriptPlayer::operator=(ScriptPlayer const&)   = default;
// ScriptModuleMinecraft::ScriptActor&  ScriptModuleMinecraft::ScriptActor::operator=(ScriptActor const&)     = default;
// Scripting::WeakLifetimeScope& Scripting::WeakLifetimeScope::operator=(Scripting::WeakLifetimeScope const&) = default;
// ScriptModuleMinecraft::ScriptPlayer::ScriptPlayer(ScriptPlayer const&)                                     = default;
// ScriptModuleMinecraft::ScriptActor::ScriptActor(ScriptActor const&)                                        = default;
// Scripting::WeakLifetimeScope::WeakLifetimeScope(WeakLifetimeScope const&)                                  = default;
// ScriptModuleMinecraft::ScriptPlayer::ScriptPlayer()                                                        = default;
// // ScriptModuleMinecraft::ScriptActor::ScriptActor() {
// //     mLevel = gsl::not_null<ServerLevel*>(static_cast<ServerLevel*>(&(ll::service::getLevel().value())));
// // };
// ScriptModuleMinecraft::ScriptActor::ScriptActor()
// : mLevel(gsl::not_null<ServerLevel*>(static_cast<ServerLevel*>(&(ll::service::getLevel().value())))) {}
// Scripting::WeakLifetimeScope::WeakLifetimeScope() = default;
// // ScriptModuleMinecraft::ScriptActor::ScriptActor()  = default;
// // Scripting::WeakLifetimeScope::~WeakLifetimeScope() = default;


namespace coral_fans::cfsp::fix {
LL_TYPE_INSTANCE_HOOK(
    ScriptActorEventListenerFix1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
    EventResult,
    ::ActorRemovedEvent const& actorRemovedEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorRemovedEvent);
#endif
    if (auto entityContext = actorRemovedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorRemovedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorEventListenerFix2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onActorCreated,
    EventResult,
    ::Actor&                    actor,
    ::ActorInitializationMethod initializationMethod
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actor, initializationMethod);
#endif
    if (fix::CFSPFixManager::getInstance().createSpMutex) {
        return EventResult::KeepGoing;
    }
    return origin(actor, initializationMethod);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorAddEffectEvent const& actorAddEffectEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorAddEffectEvent);
#endif
    if (auto entityContext = actorAddEffectEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorAddEffectEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorDefinitionEndedEvent const& actorDefinitionEndedEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorDefinitionEndedEvent);
#endif
    if (manager::CFSPManager::getInstance().tryGetCFSP(&actorDefinitionEndedEvent.mActor).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(actorDefinitionEndedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix3,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorHealEvent const& actorHealEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorHealEvent);
#endif
    if (auto entityContext = actorHealEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorHealEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix4,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorHurtEvent const& actorHurtEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorHurtEvent);
#endif
    if (auto entityContext = actorHurtEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorHurtEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix5,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorDiedEvent const& actorDiedEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorDiedEvent);
#endif
    if (auto entityContext = actorDiedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorDiedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix6,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorAttackEvent const& actorAttackEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorAttackEvent);
#endif
    if (auto entityContext = actorAttackEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorAttackEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix7,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorHealthChangedEvent const& actorHealthChangedEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorHealthChangedEvent);
#endif
    if (auto entityContext = actorHealthChangedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorHealthChangedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix8,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorDroppedItemEvent const& actorDroppedItemEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorDroppedItemEvent);
#endif
    if (auto entityContext = actorDroppedItemEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorDroppedItemEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptActorGlobalEventListenerFix9,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorAcquiredItemEvent const& actorAcquiredItemEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actorAcquiredItemEvent);
#endif
    if (manager::CFSPManager::getInstance().tryGetCFSP(&actorAcquiredItemEvent.mActor).has_value()) {
        return EventResult::KeepGoing;
    }

    return origin(actorAcquiredItemEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptLevelGlobalEventListenerFix1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelAddedPlayer,
    EventResult,
    Player& player
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player);
#endif
    if (fix::CFSPFixManager::getInstance().createSpMutex
        || manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptLevelGlobalEventListenerFix2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelRemovedPlayer,
    EventResult,
    ::Player& player
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player);
#endif
    auto& removingList = fix::CFSPFixManager::getInstance().mRemovingRecord.mRemovingSpList;
    if (auto it = removingList.find(player.mName); it != removingList.end()) {
        return EventResult::KeepGoing;
    }
    return origin(player);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptLevelGlobalEventListenerFix3,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelRemovedActor,
    EventResult,
    Actor& actor
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(actor);
#endif
    if (actor.isSimulatedPlayer()) {
        auto& removingList = fix::CFSPFixManager::getInstance().mRemovingRecord.mRemovingSpList;
        if (auto it = removingList.find(static_cast<Player*>(&actor)->mName); it != removingList.end()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actor);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptBlockGlobalEventListenerFix1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptBlockGlobalEventListener,
    &ScriptModuleMinecraft::ScriptBlockGlobalEventListener::$onBlockPlacedByPlayer,
    EventResult,
    ::Player&         player,
    ::Block const&    placedBlock,
    ::BlockPos const& pos,
    bool              isUnderwater
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player, placedBlock, pos, isUnderwater);
#endif
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, placedBlock, pos, isUnderwater);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptBlockGlobalEventListenerFix2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptBlockGlobalEventListener,
    &ScriptModuleMinecraft::ScriptBlockGlobalEventListener::$onBlockDestroyedByPlayer,
    EventResult,
    ::Player&              player,
    ::Block const&         destroyedBlock,
    ::BlockPos const&      pos,
    ::ItemStackBase const& currentItem,
    ::ItemStackBase const& itemBeforeBlockBreak
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player, destroyedBlock, pos, currentItem, itemBeforeBlockBreak);
#endif
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, destroyedBlock, pos, currentItem, itemBeforeBlockBreak);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptBlockGlobalEventListenerFix3,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptBlockGlobalEventListener,
    &ScriptModuleMinecraft::ScriptBlockGlobalEventListener::$onBlockDestructionStarted,
    EventResult,
    ::Player&         player,
    ::BlockPos const& pos,
    ::Block const&    hitBlock,
    uchar const       face,
    int const         previousProgress
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(player, pos, hitBlock, face, previousProgress);
#endif
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, pos, hitBlock, face, previousProgress);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerRespawnEvent const& respawnEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(respawnEvent);
#endif
    if (auto entityContext = respawnEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInitialSpawnEvent const& initialSpawnEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(initialSpawnEvent);
#endif
    if (auto entityContext = initialSpawnEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(initialSpawnEvent);
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix3,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerDimensionChangeAfterEvent const& playerDimensionChangeEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerDimensionChangeEvent);
#endif
    if (auto entityContext = playerDimensionChangeEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix4,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInputModeChangeEvent const& playerInputModeChangeEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerInputModeChangeEvent);
#endif
    if (auto entityContext = playerInputModeChangeEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix5,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInputPermissionCategoryChangeEvent const& event
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(event);
#endif
    if (auto entityContext = event.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix6,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInteractWithEntityAfterEvent const& playerInteractWithEntityEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerInteractWithEntityEvent);
#endif
    if (auto entityContext = playerInteractWithEntityEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix7,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInteractWithBlockAfterEvent const& playerInteractWithBlockEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerInteractWithBlockEvent);
#endif
    if (auto entityContext = playerInteractWithBlockEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix8,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerGameModeChangeEvent const& playerGameModeChangeEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerGameModeChangeEvent);
#endif
    if (auto entityContext = playerGameModeChangeEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix9,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerEmoteEvent const& playerEmoteEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(playerEmoteEvent);
#endif
    if (auto entityContext = playerEmoteEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix10,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerScriptInputEvent const& e
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(e);
#endif
    if (auto entityContext = e.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix11,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerInventoryItemChangeEvent const& e
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(e);
#endif
    if (auto entityContext = e.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix12,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerHotbarSelectedSlotChangeEvent const& e
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(e);
#endif
    if (auto entityContext = e.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix13,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerUseNameTagEvent const& nameTagEvent
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(nameTagEvent);
#endif
    if (auto entityContext = nameTagEvent.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

LL_TYPE_INSTANCE_HOOK(
    ScriptPlayerEventListenerFix14,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptPlayerEventListener,
    &ScriptModuleMinecraft::ScriptPlayerEventListener::$onEvent,
    EventResult,
    ::PlayerSwingStartEvent const& e
) {
#ifdef LL_PLAT_C
    if (auto serverInstance = ll::service::getServerInstance();
        !serverInstance
        || std::this_thread::get_id() != ll::service::getServerInstance()->mServerInstanceThread->get_id())
        return origin(e);
#endif
    if (auto entityContext = e.mPlayer->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return EventResult::KeepGoing;
}

// LL_TYPE_STATIC_HOOK(
//     CFSPSapiFixHook27,
//     ll::memory::HookPriority::Normal,
//     ScriptModuleMinecraft::ScriptActorFactory,
//     &ScriptModuleMinecraft::ScriptActorFactory::getHandle,
//     ::Scripting::StrongTypedObjectHandle<::ScriptModuleMinecraft::ScriptActor>,
//     ::Actor const&                        actor,
//     ::Scripting::WeakLifetimeScope const& scope
// ) {
//     CFSP::getInstance().getSelf().getLogger().info("ScriptActorFactory::getHandle: 1");
//     Actor* actorPtr = const_cast<Actor*>(&actor);
//     if (manager::CFSPManager::getInstance().tryGetCFSP(actorPtr).has_value()) {
//         auto res =
// static_cast<Scripting::StrongTypedObjectHandle<::ScriptModuleMinecraft::ScriptActor>*>(nullptr);
//         return *res;
//     }
//     return origin(actor, scope);
// }

// using ScriptActorQuery_GetAllPlayers_Result = ::Scripting::Result<
//     ::std::vector<::Scripting::StrongTypedObjectHandle<::ScriptModuleMinecraft::ScriptPlayer>>,
//     ::ScriptModuleMinecraft::ScriptCommandError,
//     ::Scripting::InvalidArgumentError>;

// LL_TYPE_STATIC_HOOK(
//     CFSPSapiFixHook28,
//     ll::memory::HookPriority::Normal,
//     ScriptModuleMinecraft::ScriptActorQuery,
//     &ScriptModuleMinecraft::ScriptActorQuery::getAllPlayers,
//     ScriptActorQuery_GetAllPlayers_Result,
//     ::Scripting::WeakLifetimeScope const& scope,
//     ::Level&                              level
// ) {
//     std::vector<::Scripting::StrongTypedObjectHandle<::ScriptModuleMinecraft::ScriptPlayer>> resValue;
//     level.forEachPlayer([&resValue, &scope](Player& player) {
//         if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
//             return true;
//         }
//         auto actorHandle = ScriptModuleMinecraft::ScriptActorFactory::getHandle(player, scope);
//         resValue.emplace_back(std::move(
// reinterpret_cast<Scripting::StrongTypedObjectHandle<ScriptModuleMinecraft::ScriptPlayer>&>(actorHandle)
//         ));
//         return true;
//     });
//     auto res = Scripting::ResultAny::makeResultFromAny(resValue);
//     return reinterpret_cast<ScriptActorQuery_GetAllPlayers_Result&>(res);
// }

void CFSPFixManager::sapiFix(bool enable) {
    if (enable) {
        ScriptActorEventListenerFix1::hook();
        ScriptActorEventListenerFix2::hook();
        ScriptActorGlobalEventListenerFix1::hook();
        ScriptActorGlobalEventListenerFix2::hook();
        ScriptActorGlobalEventListenerFix3::hook();
        ScriptActorGlobalEventListenerFix4::hook();
        ScriptActorGlobalEventListenerFix5::hook();
        ScriptActorGlobalEventListenerFix6::hook();
        ScriptActorGlobalEventListenerFix7::hook();
        ScriptActorGlobalEventListenerFix8::hook();
        ScriptActorGlobalEventListenerFix9::hook();
        ScriptLevelGlobalEventListenerFix1::hook();
        ScriptLevelGlobalEventListenerFix2::hook();
        ScriptLevelGlobalEventListenerFix3::hook();
        ScriptBlockGlobalEventListenerFix1::hook();
        ScriptBlockGlobalEventListenerFix2::hook();
        ScriptBlockGlobalEventListenerFix3::hook();
        ScriptPlayerEventListenerFix1::hook();
        ScriptPlayerEventListenerFix2::hook();
        ScriptPlayerEventListenerFix3::hook();
        ScriptPlayerEventListenerFix4::hook();
        ScriptPlayerEventListenerFix5::hook();
        ScriptPlayerEventListenerFix6::hook();
        ScriptPlayerEventListenerFix7::hook();
        ScriptPlayerEventListenerFix8::hook();
        ScriptPlayerEventListenerFix9::hook();
        ScriptPlayerEventListenerFix10::hook();
        ScriptPlayerEventListenerFix11::hook();
        ScriptPlayerEventListenerFix12::hook();
        ScriptPlayerEventListenerFix13::hook();
        ScriptPlayerEventListenerFix14::hook();
    } else {
        ScriptActorEventListenerFix1::unhook();
        ScriptActorEventListenerFix2::unhook();
        ScriptActorGlobalEventListenerFix1::unhook();
        ScriptActorGlobalEventListenerFix2::unhook();
        ScriptActorGlobalEventListenerFix3::unhook();
        ScriptActorGlobalEventListenerFix4::unhook();
        ScriptActorGlobalEventListenerFix5::unhook();
        ScriptActorGlobalEventListenerFix6::unhook();
        ScriptActorGlobalEventListenerFix7::unhook();
        ScriptActorGlobalEventListenerFix8::unhook();
        ScriptActorGlobalEventListenerFix9::unhook();
        ScriptLevelGlobalEventListenerFix1::unhook();
        ScriptLevelGlobalEventListenerFix2::unhook();
        ScriptLevelGlobalEventListenerFix3::unhook();
        ScriptBlockGlobalEventListenerFix1::unhook();
        ScriptBlockGlobalEventListenerFix2::unhook();
        ScriptBlockGlobalEventListenerFix3::unhook();
        ScriptPlayerEventListenerFix1::unhook();
        ScriptPlayerEventListenerFix2::unhook();
        ScriptPlayerEventListenerFix3::unhook();
        ScriptPlayerEventListenerFix4::unhook();
        ScriptPlayerEventListenerFix5::unhook();
        ScriptPlayerEventListenerFix6::unhook();
        ScriptPlayerEventListenerFix7::unhook();
        ScriptPlayerEventListenerFix8::unhook();
        ScriptPlayerEventListenerFix9::unhook();
        ScriptPlayerEventListenerFix10::unhook();
        ScriptPlayerEventListenerFix11::unhook();
        ScriptPlayerEventListenerFix12::unhook();
        ScriptPlayerEventListenerFix13::unhook();
        ScriptPlayerEventListenerFix14::unhook();
    }
    // CFSPSapiFixHook27::hook();
    // CFSPSapiFixHook28::hook();
}
} // namespace coral_fans::cfsp::fix