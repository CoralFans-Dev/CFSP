#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
base::OperateResult CFSPManager::dropSp(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.dropSp); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::success) nocheck = true;
    }
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Drop); !res) return res;
    return it->second->drop();
}

base::OperateResult CFSPManager::dropInvSp(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.dropInvSp); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::success) nocheck = true;
    }
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::DropInv); !res)
            return res;
    return it->second->dropInv();
}

base::OperateResult CFSPManager::swapSp(Player* player, std::string const& spname, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.swapSp); !checkResult)
            return checkResult;
        else if (checkResult.mType == base::OperateResult::Type::success) nocheck = true;
    }
    // check: exist
    auto it = this->mOnlineSpMap.find(spname);
    if (it == this->mOnlineSpMap.end()) {
        if (this->mOfflineSpMap.find(spname) != this->mOfflineSpMap.end())
            return base::OperateResult::error("manager.fail.spHasOffline"_tr());
        return base::OperateResult::error("manager.fail.spNotExisted"_tr());
    }
    if (!nocheck)
        // check：permission
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Swap); !res) return res;
    return it->second->swap(player);
}
} // namespace coral_fans::cfsp::manager