#pragma once

#define COMMAND_CHECK_PLAYER                                                                                           \
    auto* entity = origin.getEntity();                                                                                 \
    if (entity == nullptr || !entity->isType(ActorType::Player)) {                                                     \
        output.error("Only players can run this command");                                                             \
        return;                                                                                                        \
    }                                                                                                                  \
    auto* player = static_cast<Player*>(entity);

#define COMMAND_SIMPLAYER_CHECKPERMLIST                                                                                \
    auto& mod  = coral_fans::cfsp::mod();                                                                              \
    auto  uuid = player->getUuid().asString();                                                                         \
    switch (mod.getConfig().simPlayer.listType) {                                                                      \
    case coral_fans::cfsp::config::ListType::disabled:                                                                 \
        break;                                                                                                         \
    case coral_fans::cfsp::config::ListType::blacklist:                                                                \
        if (mod.getConfig().simPlayer.list.find(uuid) != mod.getConfig().simPlayer.list.end())                         \
            return output.error("command.sp.error.permissiondenied"_tr());                                             \
        break;                                                                                                         \
    case coral_fans::cfsp::config::ListType::whitelist:                                                                \
        if (mod.getConfig().simPlayer.list.find(uuid) == mod.getConfig().simPlayer.list.end())                         \
            return output.error("command.sp.error.permissiondenied"_tr());                                             \
        break;                                                                                                         \
    }

#ifdef CFSPEXP
#define CFSP_API __declspec(dllexport)
#else
#define CFSP_API __declspec(dllimport)
#endif

// SimPlayer reg def
#define SP_REG_DEF(NAME, ...)                                                                                          \
    CFSP_API std::pair<std::string, bool> simPlayer##NAME(Player*, std::string const&, bool, __VA_ARGS__);             \
    CFSP_API std::pair<std::string, bool> group##NAME(Player*, std::string const&, __VA_ARGS__);

// SimPlayer def with arg
#define SP_DEF_WA(NAME, ACTION, ARG_TYPE)                                                                              \
    std::pair<std::string, bool>                                                                                       \
        SimPlayerManager::simPlayer##NAME(Player* player, std::string const& spname, bool noCheck, ARG_TYPE arg) {     \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto uuid = player->getUuid();                                                                                 \
        auto it   = this->mNameSimPlayerMap.find(spname);                                                              \
        if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};            \
        if (noCheck                                                                                                    \
            || player->getCommandPermissionLevel() >= coral_fans::cfsp::mod().getConfig().simPlayer.adminPermission    \
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)                                   \
            || uuid == it->second->ownerUuid) {                                                                        \
            if (it->second->status != SimPlayerStatus::Alive)                                                          \
                return {"translate.simplayer.error.statuserror"_tr(), false};                                          \
            if (it->second->simPlayer) it->second->ACTION(arg);                                                        \
            return {"translate.simplayer.success"_tr(), true};                                                         \
        }                                                                                                              \
        return {"translate.simplayer.error.permissiondenied"_tr(), false};                                             \
    }                                                                                                                  \
    std::pair<std::string, bool> SimPlayerManager::group##NAME(                                                        \
        Player*            player,                                                                                     \
        std::string const& gname,                                                                                      \
        ARG_TYPE           arg                                                                                         \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};                    \
        auto const& UUID = player->getUuid().asString();                                                               \
        if (player->getCommandPermissionLevel() < mod().getConfig().simPlayer.adminPermission                          \
            && !mod().getConfig().simPlayer.superManagerList.contains(*player->mName) && it->second->owner != UUID     \
            && it->second->admin.find(UUID) == it->second->admin.end())                                                \
            return {"translate.simplayer.error.permissiondenied"_tr(), false};                                         \
        for (auto const& v : it->second->splist) this->simPlayer##NAME(player, v, true, arg);                          \
        return {"translate.simplayer.success"_tr(), true};                                                             \
    }

// SimPlayer def with two arg
#define SP_DEF_WA2(NAME, ACTION, ARG_TYPE1, ARG_TYPE2)                                                                 \
    std::pair<std::string, bool> SimPlayerManager::simPlayer##NAME(                                                    \
        Player*            player,                                                                                     \
        std::string const& spname,                                                                                     \
        bool               noCheck,                                                                                    \
        ARG_TYPE1          arg1,                                                                                       \
        ARG_TYPE2          arg2                                                                                        \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto uuid = player->getUuid();                                                                                 \
        auto it   = this->mNameSimPlayerMap.find(spname);                                                              \
        if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};            \
        if (noCheck                                                                                                    \
            || player->getCommandPermissionLevel() >= coral_fans::cfsp::mod().getConfig().simPlayer.adminPermission    \
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)                                   \
            || uuid == it->second->ownerUuid) {                                                                        \
            if (it->second->status != SimPlayerStatus::Alive)                                                          \
                return {"translate.simplayer.error.statuserror"_tr(), false};                                          \
            if (it->second->simPlayer) it->second->ACTION(arg1, arg2);                                                 \
            return {"translate.simplayer.success"_tr(), true};                                                         \
        }                                                                                                              \
        return {"translate.simplayer.error.permissiondenied"_tr(), false};                                             \
    }                                                                                                                  \
    std::pair<std::string, bool>                                                                                       \
        SimPlayerManager::group##NAME(Player* player, std::string const& gname, ARG_TYPE1 arg1, ARG_TYPE2 arg2) {      \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};                    \
        auto const& UUID = player->getUuid().asString();                                                               \
        if (player->getCommandPermissionLevel() < mod().getConfig().simPlayer.adminPermission                          \
            && !mod().getConfig().simPlayer.superManagerList.contains(*player->mName) && it->second->owner != UUID     \
            && it->second->admin.find(UUID) == it->second->admin.end())                                                \
            return {"translate.simplayer.error.permissiondenied"_tr(), false};                                         \
        for (auto const& v : it->second->splist) this->simPlayer##NAME(player, v, true, arg1, arg2);                   \
        return {"translate.simplayer.success"_tr(), true};                                                             \
    }

// SimPlayer def
#define SP_DEF(NAME, ACTION)                                                                                           \
    std::pair<std::string, bool> SimPlayerManager::simPlayer##NAME(                                                    \
        Player*            player,                                                                                     \
        std::string const& spname,                                                                                     \
        bool               noCheck                                                                                     \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto uuid = player->getUuid();                                                                                 \
        auto it   = this->mNameSimPlayerMap.find(spname);                                                              \
        if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};            \
        if (noCheck                                                                                                    \
            || player->getCommandPermissionLevel() >= coral_fans::cfsp::mod().getConfig().simPlayer.adminPermission    \
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)                                   \
            || uuid == it->second->ownerUuid) {                                                                        \
            if (it->second->status != SimPlayerStatus::Alive)                                                          \
                return {"translate.simplayer.error.statuserror"_tr(), false};                                          \
            if (it->second->simPlayer) it->second->ACTION();                                                           \
            return {"translate.simplayer.success"_tr(), true};                                                         \
        }                                                                                                              \
        return {"translate.simplayer.error.permissiondenied"_tr(), false};                                             \
    }                                                                                                                  \
    std::pair<std::string, bool> SimPlayerManager::group##NAME(Player* player, std::string const& gname) {             \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};                    \
        auto const& UUID = player->getUuid().asString();                                                               \
        if (player->getCommandPermissionLevel() < mod().getConfig().simPlayer.adminPermission                          \
            && !mod().getConfig().simPlayer.superManagerList.contains(*player->mName) && it->second->owner != UUID     \
            && it->second->admin.find(UUID) == it->second->admin.end())                                                \
            return {"translate.simplayer.error.permissiondenied"_tr(), false};                                         \
        for (auto const& v : it->second->splist) this->simPlayer##NAME(player, v, true);                               \
        return {"translate.simplayer.success"_tr(), true};                                                             \
    }

// SimPlayer def task with arg
#define SP_DEF_TASK_WA(NAME, ACTION, ARG_TYPE)                                                                         \
    std::pair<std::string, bool> SimPlayerManager::simPlayer##NAME(                                                    \
        Player*            player,                                                                                     \
        std::string const& spname,                                                                                     \
        bool               noCheck,                                                                                    \
        ARG_TYPE           arg,                                                                                        \
        int                interval,                                                                                   \
        int                times                                                                                       \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto uuid = player->getUuid();                                                                                 \
        auto it   = this->mNameSimPlayerMap.find(spname);                                                              \
        if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};            \
        if (noCheck                                                                                                    \
            || player->getCommandPermissionLevel() >= coral_fans::cfsp::mod().getConfig().simPlayer.adminPermission    \
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)                                   \
            || uuid == it->second->ownerUuid) {                                                                        \
            if (it->second->status != SimPlayerStatus::Alive)                                                          \
                return {"translate.simplayer.error.statuserror"_tr(), false};                                          \
            if (!it->second->isFree()) return {"translate.simplayer.error.nonfree"_tr(), false};                       \
            if (it->second->simPlayer) it->second->ACTION(arg);                                                        \
            if (interval >= 1) {                                                                                       \
                it->second->taskid =                                                                                   \
                    this->mScheduler->add(interval, [times, sp = it->second, arg](unsigned long long t) mutable {      \
                        if (times > 0 && t > (unsigned long long)times - 1) return false;                              \
                        if (sp->simPlayer) [[likely]] {                                                                \
                            sp->ACTION(arg);                                                                           \
                            return true;                                                                               \
                        }                                                                                              \
                        return false;                                                                                  \
                    });                                                                                                \
            } else return {"translate.simplayer.error.nonpositive"_tr(), false};                                       \
            return {"translate.simplayer.success"_tr(), true};                                                         \
        }                                                                                                              \
        return {"translate.simplayer.error.permissiondenied"_tr(), false};                                             \
    }                                                                                                                  \
    std::pair<std::string, bool> SimPlayerManager::group##NAME(                                                        \
        Player*            player,                                                                                     \
        std::string const& gname,                                                                                      \
        ARG_TYPE           arg,                                                                                        \
        int                interval,                                                                                   \
        int                times                                                                                       \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};                    \
        auto const& UUID = player->getUuid().asString();                                                               \
        if (player->getCommandPermissionLevel() < mod().getConfig().simPlayer.adminPermission                          \
            && !mod().getConfig().simPlayer.superManagerList.contains(*player->mName) && it->second->owner != UUID     \
            && it->second->admin.find(UUID) == it->second->admin.end())                                                \
            return {"translate.simplayer.error.permissiondenied"_tr(), false};                                         \
        for (auto const& v : it->second->splist) this->simPlayer##NAME(player, v, true, arg, interval, times);         \
        return {"translate.simplayer.success"_tr(), true};                                                             \
    }

// SimPlayer def task
#define SP_DEF_TASK(NAME, ACTION)                                                                                      \
    std::pair<std::string, bool> SimPlayerManager::simPlayer##NAME(                                                    \
        Player*            player,                                                                                     \
        std::string const& spname,                                                                                     \
        bool               noCheck,                                                                                    \
        int                interval,                                                                                   \
        int                times                                                                                       \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto uuid = player->getUuid();                                                                                 \
        auto it   = this->mNameSimPlayerMap.find(spname);                                                              \
        if (it == this->mNameSimPlayerMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};            \
        if (noCheck                                                                                                    \
            || player->getCommandPermissionLevel() >= coral_fans::cfsp::mod().getConfig().simPlayer.adminPermission    \
            || mod().getConfig().simPlayer.superManagerList.contains(*player->mName)                                   \
            || uuid == it->second->ownerUuid) {                                                                        \
            if (it->second->status != SimPlayerStatus::Alive)                                                          \
                return {"translate.simplayer.error.statuserror"_tr(), false};                                          \
            if (!it->second->isFree()) return {"translate.simplayer.error.nonfree"_tr(), false};                       \
            if (it->second->simPlayer) it->second->ACTION();                                                           \
            if (interval >= 1) {                                                                                       \
                it->second->taskid =                                                                                   \
                    this->mScheduler->add(interval, [times, sp = it->second](unsigned long long t) mutable {           \
                        if (times > 0 && t > (unsigned long long)times - 1) return false;                              \
                        if (sp->simPlayer) [[likely]] {                                                                \
                            sp->ACTION();                                                                              \
                            return true;                                                                               \
                        }                                                                                              \
                        return false;                                                                                  \
                    });                                                                                                \
            } else return {"translate.simplayer.error.nonpositive"_tr(), false};                                       \
            return {"translate.simplayer.success"_tr(), true};                                                         \
        }                                                                                                              \
        return {"translate.simplayer.error.permissiondenied"_tr(), false};                                             \
    }                                                                                                                  \
    std::pair<std::string, bool>                                                                                       \
        SimPlayerManager::group##NAME(Player* player, std::string const& gname, int interval, int times) {             \
        using ll::i18n_literals::operator""_tr;                                                                        \
        auto it = this->mGroupMap.find(gname);                                                                         \
        if (it == this->mGroupMap.end()) return {"translate.simplayer.error.notfound"_tr(), false};                    \
        auto const& UUID = player->getUuid().asString();                                                               \
        if (player->getCommandPermissionLevel() < mod().getConfig().simPlayer.adminPermission                          \
            && !mod().getConfig().simPlayer.superManagerList.contains(*player->mName) && it->second->owner != UUID     \
            && it->second->admin.find(UUID) == it->second->admin.end())                                                \
            return {"translate.simplayer.error.permissiondenied"_tr(), false};                                         \
        for (auto const& v : it->second->splist) this->simPlayer##NAME(player, v, true, interval, times);              \
        return {"translate.simplayer.success"_tr(), true};                                                             \
    }

#define LUAAPI(name) int lua_api_##name(lua_State* L)

#define LUA_ARG_COUNT_CHECK_C(i)                                                                                       \
    if (lua_gettop(L) != (i)) return luaL_error(L, "%d args expected", (i));

#define LUA_ARG_COUNT_CHECK_M(i)                                                                                       \
    if (lua_gettop(L) != (i) + 1) return luaL_error(L, "%d args expected (without \"self\")", (i));
