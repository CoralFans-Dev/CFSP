#include "CFSPFixManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/ecs./WeakEntityRef.h"
#include "mc/scripting/modules/minecraft/actor/ScriptActor.h"
#include "mc/scripting/modules/minecraft/events/ScriptActorEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptActorGlobalEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptBlockGlobalEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptLevelGlobalEventListener.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/events/ActorAttackEvent.h"
#include "mc/world/events/ActorDiedEvent.h"
#include "mc/world/events/ActorHealthChangedEvent.h"
#include "mc/world/events/ActorHurtEvent.h"
#include "mc/world/events/ActorRemovedEvent.h"
#include "mc/world/events/EventResult.h"
#include "mc/world/events/ProjectileHitEvent.h"


namespace coral_fans::cfsp::fix {

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook1,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
    EventResult,
    ::ActorRemovedEvent const& actorRemovedEvent
) {
    if (auto entityContext = actorRemovedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorRemovedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook2,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
    EventResult,
    ::ActorAttackEvent const& actorAttackEvent
) {
    if (auto entityContext = actorAttackEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorAttackEvent);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook3,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorHurtEvent const& actorHurtEvent
) {
    if (auto entityContext = actorHurtEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorHurtEvent);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook4,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
    EventResult,
    ::ActorHealthChangedEvent const& actorHealthChangedEvent
) {
    if (auto entityContext = actorHealthChangedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorHealthChangedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook5,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorGlobalEventListener,
    &ScriptModuleMinecraft::ScriptActorGlobalEventListener::$onEvent,
    EventResult,
    ::ActorDiedEvent const& actorDiedEvent
) {
    if (auto entityContext = actorDiedEvent.mEntity->lock()) {
        auto actor = Actor::tryGetFromEntity(*entityContext, false);
        if (actor && manager::CFSPManager::getInstance().tryGetCFSP(actor).has_value()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actorDiedEvent);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook6,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onActorCreated,
    EventResult,
    ::Actor&                    actor,
    ::ActorInitializationMethod initializationMethod
) {
    if (fix::CFSPFixManager::getInstance().createSpMutex) {
        return EventResult::KeepGoing;
    }
    return origin(actor, initializationMethod);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook7,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelRemovedPlayer,
    EventResult,
    ::Player& player
) {
    auto& removingList = fix::CFSPFixManager::getInstance().mRemovingRecord.mRemovingSpList;
    if (auto it = removingList.find(player.mName); it != removingList.end()) {
        return EventResult::KeepGoing;
    }
    return origin(player);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook8,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelAddedPlayer,
    EventResult,
    Player& player
) {
    if (fix::CFSPFixManager::getInstance().createSpMutex
        || manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook9,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptLevelGlobalEventListener,
    &ScriptModuleMinecraft::ScriptLevelGlobalEventListener::$onLevelRemovedActor,
    EventResult,
    Actor& actor
) {
    if (actor.isSimulatedPlayer()) {
        auto& removingList = fix::CFSPFixManager::getInstance().mRemovingRecord.mRemovingSpList;
        if (auto it = removingList.find(static_cast<Player*>(&actor)->mName); it != removingList.end()) {
            return EventResult::KeepGoing;
        }
    }
    return origin(actor);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook10,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptBlockGlobalEventListener,
    &ScriptModuleMinecraft::ScriptBlockGlobalEventListener::$onBlockPlacedByPlayer,
    EventResult,
    ::Player&         player,
    ::Block const&    placedBlock,
    ::BlockPos const& pos,
    bool              isUnderwater
) {
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, placedBlock, pos, isUnderwater);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook11,
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
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, destroyedBlock, pos, currentItem, itemBeforeBlockBreak);
}

LL_TYPE_INSTANCE_HOOK(
    CFSPSapiFixHook12,
    ll::memory::HookPriority::Normal,
    ScriptModuleMinecraft::ScriptBlockGlobalEventListener,
    &ScriptModuleMinecraft::ScriptBlockGlobalEventListener::$onBlockDestructionStarted,
    EventResult,
    ::Player&         player,
    ::BlockPos const& pos,
    ::Block const&    hitBlock,
    uchar const       face
) {
    if (manager::CFSPManager::getInstance().tryGetCFSP(&player).has_value()) {
        return EventResult::KeepGoing;
    }
    return origin(player, pos, hitBlock, face);
}

void CFSPFixManager::sapiFix() {
    CFSPSapiFixHook1::hook();
    CFSPSapiFixHook2::hook();
    CFSPSapiFixHook3::hook();
    CFSPSapiFixHook4::hook();
    CFSPSapiFixHook5::hook();
    CFSPSapiFixHook6::hook();
    CFSPSapiFixHook7::hook();
    CFSPSapiFixHook8::hook();
    CFSPSapiFixHook9::hook();
    CFSPSapiFixHook10::hook();
    CFSPSapiFixHook11::hook();
    CFSPSapiFixHook12::hook();
}
} // namespace coral_fans::cfsp::fix