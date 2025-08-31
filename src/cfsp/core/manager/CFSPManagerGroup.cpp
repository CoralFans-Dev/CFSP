#include "CFSPManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::manager {
base::OperateResult CFSPManager::createGroup(Player* player, std::string const& gname) {
    using ll::i18n_literals::operator""_tr;
    if (auto checkResult = this->canCreateGroup(player)) return checkResult;
    if (this->mGroupMap.find(gname) != this->mGroupMap.end())
        return base::OperateResult::error("manager.fail.groupHasExisted"_tr());

    group::GroupData groupData;
    groupData.name      = gname;
    groupData.ownerUuid = player->getUuid().asString();
    this->mGroupMap.emplace(gname, std::make_shared<group::CFSPGroup>(groupData));

    ll::command::CommandRegistrar::getInstance().addSoftEnumValues("cfspGroup", {gname});
    return base::OperateResult::success("manager.success.create"_tr());
}
} // namespace coral_fans::cfsp::manager