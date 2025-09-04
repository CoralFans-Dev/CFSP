#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
base::OperateResult
CFSPManager::spAttack(Player* player, std::string const& spname, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spAttack); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Attack); !res)
            return res;
    return it->second->attack(times, interval);
}

base::OperateResult
CFSPManager::spBuild(Player* player, std::string const& spname, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spBuild); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Build); !res)
            return res;
    return it->second->build(times, interval);
}

base::OperateResult
CFSPManager::spInteract(Player* player, std::string const& spname, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spInteract); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Interact); !res)
            return res;
    return it->second->interact(times, interval);
}

base::OperateResult
CFSPManager::spJump(Player* player, std::string const& spname, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spJump); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Jump); !res) return res;
    return it->second->jump(times, interval);
}

base::OperateResult
CFSPManager::spUse(Player* player, std::string const& spname, int _long, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spUse); !checkResult) return checkResult;
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Use); !res) return res;
    return it->second->use(_long, times, interval);
}

base::OperateResult
CFSPManager::spDestroy(Player* player, std::string const& spname, int _long, int times, int interval, bool nocheck) {
    using ll::i18n_literals::operator""_tr;
    if (!nocheck) {
        if (auto checkResult = this->baseCheck(player, this->mPermissionConfig.spDestroy); !checkResult)
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
        if (auto res = it->second->hasPermission(player, simulated_player::SimPlayerPermission::Destroy); !res)
            return res;
    return it->second->destroy(_long, times, interval);
}
} // namespace coral_fans::cfsp::manager