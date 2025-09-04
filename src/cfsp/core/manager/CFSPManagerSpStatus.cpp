#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include <optional>

namespace coral_fans::cfsp::manager {
base::OperateResult
CFSPManager::spSneaking(Player* player, std::string const& spname, std::optional<bool> enabled, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSneaking); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Sneaking); !res)
            return res;
    return it->second->sneaking(enabled);
}

base::OperateResult
CFSPManager::spSwimming(Player* player, std::string const& spname, std::optional<bool> enabled, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSwimming); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Swimming); !res)
            return res;
    return it->second->swimming(enabled);
}

base::OperateResult
CFSPManager::spFlying(Player* player, std::string const& spname, std::optional<bool> enabled, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spFlying); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Flying); !res)
            return res;
    return it->second->flying(enabled);
}

base::OperateResult
CFSPManager::spSprinting(Player* player, std::string const& spname, std::optional<bool> enabled, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spSprinting); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Sprinting); !res)
            return res;
    return it->second->sprinting(enabled);
}
} // namespace coral_fans::cfsp::manager