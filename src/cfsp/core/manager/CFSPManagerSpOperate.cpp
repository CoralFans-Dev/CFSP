#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
#define SP_OPERATE_DEF(FUNC, ACTION)                                                                                   \
    base::OperateResult                                                                                                \
        CFSPManager::sp##FUNC(Player* player, std::string const& spname, int times, int interval, bool nocheck) {      \
        using ll::i18n_literals::operator""_tr;                                                                        \
        if (!nocheck) {                                                                                                \
            if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.sp##FUNC); !checkResult)            \
                return checkResult;                                                                                    \
            else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;                          \
        }                                                                                                              \
        auto it = this->mOnlineSpMap.find(spname);                                                                     \
        if (it == this->mOnlineSpMap.end()) {                                                                          \
            if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())                                         \
                return base::OperateResult::error("manager.fail.spHasOffline"_tr());                                   \
            return base::OperateResult::error("manager.fail.spNotExisted"_tr());                                       \
        }                                                                                                              \
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::FUNC))               \
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());                                   \
        return it->second->ACTION(times, interval);                                                                    \
    }

#define SP_OPERATE_DEF2(FUNC, ACTION)                                                                                  \
    base::OperateResult CFSPManager::sp##FUNC(                                                                         \
        Player*            player,                                                                                     \
        std::string const& spname,                                                                                     \
        int                _long,                                                                                      \
        int                times,                                                                                      \
        int                interval,                                                                                   \
        bool               nocheck                                                                                     \
    ) {                                                                                                                \
        using ll::i18n_literals::operator""_tr;                                                                        \
        if (!nocheck) {                                                                                                \
            if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.sp##FUNC); !checkResult)            \
                return checkResult;                                                                                    \
            else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;                          \
        }                                                                                                              \
        auto it = this->mOnlineSpMap.find(spname);                                                                     \
        if (it == this->mOnlineSpMap.end()) {                                                                          \
            if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())                                         \
                return base::OperateResult::error("manager.fail.spHasOffline"_tr());                                   \
            return base::OperateResult::error("manager.fail.spNotExisted"_tr());                                       \
        }                                                                                                              \
        if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::FUNC))               \
            return base::OperateResult::error("manager.fail.permissionDenied"_tr());                                   \
        return it->second->ACTION(_long, times, interval);                                                             \
    }

SP_OPERATE_DEF(Attack, attack)
SP_OPERATE_DEF(Build, build)
SP_OPERATE_DEF(Interact, interact)
SP_OPERATE_DEF(Jump, jump)
SP_OPERATE_DEF2(Use, use)
SP_OPERATE_DEF2(Destroy, destroy)
} // namespace coral_fans::cfsp::manager