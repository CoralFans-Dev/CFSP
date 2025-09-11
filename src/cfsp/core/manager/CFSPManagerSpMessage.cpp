#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
#define SP_MESSAGE_DEF(FUNC, ACTION)                                                                                   \
    base::OperateResult                                                                                                \
        CFSPManager::sp##FUNC(Player* player, std::string const& spname, std::string message, bool nocheck) {          \
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
        return it->second->ACTION(message);                                                                            \
    }

SP_MESSAGE_DEF(Chat, chat)
SP_MESSAGE_DEF(RunCmd, runcmd)
} // namespace coral_fans::cfsp::manager