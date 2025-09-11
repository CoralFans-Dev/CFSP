#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
#define SP_INV_DEF(FUNC, ACTION)                                                                                       \
    base::OperateResult CFSPManager::sp##FUNC(Player* player, std::string const& spname, bool nocheck) {               \
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
        return it->second->ACTION;                                                                                     \
    }

base::OperateResult CFSPManager::spInvInfo(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (!isAllowed(player)) return base::OperateResult::error("manager.fail.permissionDenied"_tr());
        if (isManager(player)) nocheck = true;
    }
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::None))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return it->second->invInfo();
}

SP_INV_DEF(Drop, drop())
SP_INV_DEF(DropInv, dropInv())
SP_INV_DEF(Swap, swap(player))

base::OperateResult CFSPManager::spSelect(Player* player, std::string const& spname, int id, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSelect); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::Success) nocheck = true;
    }
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck && !it->second->hasPermission(player, simulated_player::SimPlayerPermission::Select))
        return base::OperateResult::error("manager.fail.permissionDenied"_tr());
    return it->second->select(id);
}
} // namespace coral_fans::cfsp::manager