#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include <optional>

namespace coral_fans::cfsp::manager {
#define SP_MOVE_DEF(FUNC, ACTION)                                                                                      \
    base::OperateResult                                                                                                \
        CFSPManager::sp##FUNC(Player* player, std::string const& spname, Vec3 const& pos, bool nocheck) {              \
        using ll::i18n_literals::operator""_tr;                                                                        \
        if (!nocheck) {                                                                                                \
            if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.sp##FUNC); !checkResult)            \
                return checkResult;                                                                                    \
            else if (checkResult.mType == base::OperateResult::Type::success) nocheck = true;                          \
        }                                                                                                              \
        auto it = this->mOnlineSpMap.find(spname);                                                                     \
        if (it == this->mOnlineSpMap.end()) {                                                                          \
            if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())                                         \
                return base::OperateResult::error("manager.fail.spHasOffline"_tr());                                   \
            return base::OperateResult::error("manager.fail.spNotExisted"_tr());                                       \
        }                                                                                                              \
        if (!nocheck)                                                                                                  \
            if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::FUNC); !res)       \
                return res;                                                                                            \
        return it->second->ACTION(pos);                                                                                \
    }

SP_MOVE_DEF(MoveTo, moveTo)
SP_MOVE_DEF(NavTo, navTo)

base::OperateResult
CFSPManager::spTp(Player* player, std::string const& spname, Vec3 const& pos, std::optional<int> dimId, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spTp); !checkResult) return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::success) nocheck = true;
    }
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck)
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Tp); !res) return res;
    return it->second->tp(pos, dimId);
}
} // namespace coral_fans::cfsp::manager