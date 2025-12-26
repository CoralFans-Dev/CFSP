#include "CFSPFixManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/ecs./WeakEntityRef.h"
#include "mc/scripting/modules/minecraft/actor/ScriptActor.h"
#include "mc/scripting/modules/minecraft/events/ScriptActorEventListener.h"
#include "mc/scripting/modules/minecraft/events/ScriptLevelEventListener.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/events/ActorAttackEvent.h"
#include "mc/world/events/ActorDiedEvent.h"
#include "mc/world/events/ActorHealthChangedEvent.h"
#include "mc/world/events/ActorHurtEvent.h"
#include "mc/world/events/ActorRemovedEvent.h"
#include "mc/world/events/EventResult.h"
#include "mc/world/events/ProjectileHitEvent.h"
#include <string>

namespace coral_fans::cfsp::fix {
std::unordered_multiset<std::string> mRemovingSpSet;

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
            mRemovingSpSet.insert(static_cast<Player*>(actor)->mName);
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
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
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
    ScriptModuleMinecraft::ScriptActorEventListener,
    &ScriptModuleMinecraft::ScriptActorEventListener::$onEvent,
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
    ScriptModuleMinecraft::ScriptLevelEventListener,
    &ScriptModuleMinecraft::ScriptLevelEventListener::$onLevelRemovedPlayer,
    EventResult,
    ::Player& player
) {
    if (auto it = mRemovingSpSet.find(player.mName); it != mRemovingSpSet.end()) {
        mRemovingSpSet.erase(it);
        return EventResult::KeepGoing;
    }
    return origin(player);
}

void CFSPFixManager::sapiFix() {
    CFSPSapiFixHook1::hook();
    CFSPSapiFixHook2::hook();
    CFSPSapiFixHook3::hook();
    CFSPSapiFixHook4::hook();
    CFSPSapiFixHook5::hook();
    CFSPSapiFixHook6::hook();
    CFSPSapiFixHook7::hook();
}
} // namespace coral_fans::cfsp::fix