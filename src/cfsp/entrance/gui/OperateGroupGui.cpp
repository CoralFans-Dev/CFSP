#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/base/Utils.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"
#include "mc/world/phys/HitResult.h"
#include <vector>


namespace coral_fans::cfsp::gui {
void GuiManager::sendOperateGroupPage(Player& player, std::shared_ptr<group::CFSPGroup> group) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm(group->mData.name);
    uint perm;
    bool ismanager = manager::CFSPManager::getInstance().isManager(&player);
    if (ismanager) perm = manager::CFSPManager::getInstance().getGroupPermissionMask();
    else
        perm = (uint)group->getPermission(&player)
             & manager::CFSPManager::getInstance().getGroupPermissionMask(
                 std::optional<CommandPermissionLevel>(player.getCommandPermissionLevel())
             );
    if (perm)
        form.appendButton("gui.operateGroup.groupInfo"_tr(), [this, group](Player& player) {
            this->sendGroupInfoPage(player, group);
        });
    if (perm & (uint)group::GroupPermission::AddSp || perm & (uint)group::GroupPermission::RmSp)
        form.appendButton("gui.operateGroup.manageSp"_tr(), [this, group, perm](Player& player) {
            this->sendManageSpInGroupPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::Spawn)
        form.appendButton("gui.operateGroup.online"_tr(), [gname = group->mData.name](Player& player) {
            auto res = manager::CFSPManager::getInstance().groupSpawn(&player, gname);
            for (auto perRes : res) perRes.sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Despawn)
        form.appendButton("gui.operateGroup.offline"_tr(), [gname = group->mData.name](Player& player) {
            auto res = manager::CFSPManager::getInstance().groupDespawn(&player, gname);
            for (auto perRes : res) perRes.sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Respawn)
        form.appendButton("gui.operateGroup.respawn"_tr(), [gname = group->mData.name](Player& player) {
            auto res = manager::CFSPManager::getInstance().groupRespawn(&player, gname);
            for (auto perRes : res) perRes.sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Tp)
        form.appendButton("gui.operateGroup.tp"_tr(), [this, group](Player& player) {
            Vec3        pos;
            const auto& hit = player.traceRay(5.25f, false, true);
            if (hit) pos = hit.mPos;
            else pos = player.getFeetPos();
            this->sendGroupTpOperatorPage(
                player,
                group,
                player.getDimensionId(),
                std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
            );
        });
    if (perm)
        form.appendButton("gui.operateGroup.inv"_tr(), [this, group, perm](Player& player) {
            this->sendGroupInvOperatorPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::LookAt)
        form.appendButton("gui.operateGroup.lookat"_tr(), [this, group](Player& player) {
            Vec3        pos;
            const auto& hit = player.traceRay(5.25f, false, true);
            if (hit) pos = hit.mPos;
            else pos = player.getFeetPos();
            this->sendGroupLookOperatorPage(
                player,
                group,
                std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
            );
        });
    if (perm & (uint)group::GroupPermission::MoveTo || perm & (uint)group::GroupPermission::NavTo)
        form.appendButton("gui.operateGroup.move"_tr(), [this, group, perm](Player& player) {
            Vec3        pos;
            const auto& hit = player.traceRay(5.25f, false, true);
            if (hit) pos = hit.mPos;
            else pos = player.getFeetPos();
            this->sendGroupMoveOperatorPage(
                player,
                group,
                perm,
                std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
            );
        });
    if (perm & (uint)group::GroupPermission::Attack || perm & (uint)group::GroupPermission::Build
        || perm & (uint)group::GroupPermission::Interact || perm & (uint)group::GroupPermission::Jump)
        form.appendButton("gui.operateGroup.perform"_tr(), [this, group, perm](Player& player) {
            this->sendGroupActionOperatorPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::Use || perm & (uint)group::GroupPermission::Destroy)
        form.appendButton("gui.operateGroup.perform2"_tr(), [this, group, perm](Player& player) {
            this->sendGroupLongActionOperatorPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::Stop)
        form.appendButton("gui.operateGroup.stop"_tr(), [gname = group->mData.name](Player& player) {
            auto res = manager::CFSPManager::getInstance().groupStop(&player, gname);
            for (auto perRes : res) perRes.sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::Sneaking || perm & (uint)group::GroupPermission::Swimming
        || perm & (uint)group::GroupPermission::Flying || perm & (uint)group::GroupPermission::Sprinting)
        form.appendButton("gui.operateGroup.status"_tr(), [this, group, perm](Player& player) {
            this->sendGroupStatusOperatorPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::Chat || perm & (uint)group::GroupPermission::RunCmd)
        form.appendButton("gui.operateGroup.message"_tr(), [this, group, perm](Player& player) {
            this->sendGroupMessageOperatorPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::Delete)
        form.appendButton("gui.operateGroup.rm"_tr(), [this, group](Player& player) {
            auto confirmForm = ll::form::ModalForm(
                "gui.operateGroup.rm"_tr(),
                "gui.operateGroup.confirmRm"_tr(group->mData.name),
                "base.yesOrNo.yes"_tr(),
                "base.yesOrNo.no"_tr()
            );
            confirmForm.sendTo(
                player,
                [this,
                 group](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        return this->sendOperateGroupPage(player, group);
                    if (!result.has_value()) return;

                    if (result.value() == ll::form::ModalFormSelectedButton::Upper) {
                        return manager::CFSPManager::getInstance()
                            .groupDelete(&player, group->mData.name)
                            .sendTo(player);
                    } else if (result.value() == ll::form::ModalFormSelectedButton::Lower)
                        return this->sendOperateGroupPage(player, group);
                }
            );
        });
    if (perm & (uint)group::GroupPermission::DeleteSp)
        form.appendButton("gui.operateGroup.deleteSp"_tr(), [this, group](Player& player) {
            auto confirmForm = ll::form::ModalForm(
                "gui.operateGroup.deleteSp"_tr(),
                "gui.operateGroup.confirmDeleteSp"_tr(group->mData.name),
                "base.yesOrNo.yes"_tr(),
                "base.yesOrNo.no"_tr()
            );
            confirmForm.sendTo(
                player,
                [this,
                 group](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        return this->sendOperateGroupPage(player, group);
                    if (!result.has_value()) return;

                    if (result.value() == ll::form::ModalFormSelectedButton::Upper) {
                        auto res = manager::CFSPManager::getInstance().groupDeleteSp(&player, group->mData.name);
                        for (auto& perRes : res)
                            if (perRes.mType != base::OperateResult::Type::Swing) perRes.sendTo(player);
                        if (!group->mData.splist.empty()) {
                            std::unordered_set<std::string> confirmList = group->mData.splist;
                            this->sendGroupDeleteSpConfrim(player, confirmList);
                        }
                    } else if (result.value() == ll::form::ModalFormSelectedButton::Lower)
                        return this->sendOperateGroupPage(player, group);
                }
            );
        });
    if (group->mData.ownerUuid == player.getUuid().asString() || ismanager)
        form.appendButton("gui.operateGroup.perm"_tr(), [this, group](Player& player) {
            this->sendGroupPermPage(player, group);
        });

    form.sendTo(player);
}

void GuiManager::sendGroupDeleteSpConfrim(Player& player, std::unordered_set<std::string> confirmList) {
    using ll::i18n_literals::operator""_tr;
    auto        it     = confirmList.begin();
    std::string spname = it->data();
    confirmList.erase(it);
    auto confirmForm = ll::form::ModalForm(
        "gui.operateGroup.deleteSp"_tr(),
        "gui.operateGroup.confirmDeleteSp2"_tr(spname),
        "base.yesOrNo.yes"_tr(),
        "base.yesOrNo.no"_tr()
    );
    confirmForm.sendTo(
        player,
        [this,
         spname,
         confirmList](Player& player, ll::form::ModalFormResult result, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed) return;
            if (!result.has_value()) return;

            if (result.value() == ll::form::ModalFormSelectedButton::Upper)
                manager::CFSPManager::getInstance().spDelete(&player, spname, true, true).sendTo(player);
            if (!confirmList.empty()) this->sendGroupDeleteSpConfrim(player, confirmList);
        }
    );
}

void GuiManager::sendCreateGroupPage(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.managesp.title"_tr());
    form.appendInput("name", "gui.creategroup.title"_tr());
    form.sendTo(
        player,
        [this](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("name");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second)) {
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            }
            auto name = std::get<std::string>(it->second);

            auto res = manager::CFSPManager::getInstance().groupCreate(&player, name);
            res.sendTo(player);
            if (res)
                this->sendManageSpInGroupPage(
                    player,
                    manager::CFSPManager::getInstance().tryGetCFSPGroup(name).value(),
                    (uint)-1
                );
        }
    );
}

void GuiManager::sendManageSpInGroupPage(Player& player, std::shared_ptr<group::CFSPGroup> group, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.manageSpInGroup.title"_tr());
    std::vector<std::string> splist;
    if (manager::CFSPManager::getInstance().isManager(&player))
        splist = manager::CFSPManager::getInstance().getAllSpNamesSorted();
    else splist = manager::CFSPManager::getInstance().getCanBeAddedSpList(&player);
    for (auto const& spname : splist) {
        bool isContain = group->mData.splist.contains(spname);
        if (isContain) {
            if (perm & (uint)group::GroupPermission::RmSp) form.appendToggle(spname, spname, isContain);
        } else if (perm & (uint)group::GroupPermission::AddSp) form.appendToggle(spname, spname, isContain);
    }
    form.sendTo(
        player,
        [this,
         splist,
         group,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            std::vector<std::string> addList, removeList;
            for (auto const& spname : splist)
                if (auto it = elements.value().find(spname);
                    it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                    if ((bool)std::get<uint64>(it->second)) {
                        if (!group->mData.splist.contains(spname)) addList.emplace_back(spname);
                    } else if (group->mData.splist.contains(spname)) removeList.emplace_back(spname);
                }
            std::vector<base::OperateResult> res;
            if (perm & (uint)group::GroupPermission::AddSp)
                for (auto const& spname : addList)
                    res.emplace_back(manager::CFSPManager::getInstance().groupAddSp(&player, group->mData.name, spname)
                    );
            if (perm & (uint)group::GroupPermission::RmSp)
                for (auto const& spname : removeList)
                    res.emplace_back(manager::CFSPManager::getInstance().groupRmSp(&player, group->mData.name, spname));
            for (auto& perRes : res) perRes.sendTo(player);
        }
    );
}

void GuiManager::sendGroupInfoPage(Player& player, std::shared_ptr<group::CFSPGroup> group) {
    using ll::i18n_literals::operator""_tr;
    std::string info = "";
    for (auto perRes : manager::CFSPManager::getInstance().groupInfo(&player, group->mData.name))
        info += perRes.mInfo + '\n';
    ll::form::SimpleForm("gui.info.groupTitle"_tr(), info)
        .sendTo(player, [this, group](Player& player, int, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                this->sendOperateGroupPage(player, group);
        });
}

void GuiManager::sendGroupTpOperatorPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    int                               defDim,
    std::string                       defPos
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.tp.groupTitle"_tr());
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
    form.appendDropdown(
        "dim",
        "gui.para.targetDim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
    form.sendTo(
        player,
        [this,
         group](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("dim");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleDim = std::get<std::string>(it->second);
            int  dim;
            if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
            else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
            else if (eleDim == "base.dimension.theend"_tr()) dim = 2;
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendGroupTpOperatorPage(player, group, dim, elePos);
            }
            for (auto& perRes :
                 manager::CFSPManager::getInstance().groupTp(&player, group->mData.name, targetPos.value(), dim))
                perRes.sendTo(player);
        }
    );
}

void GuiManager::sendGroupLookOperatorPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    std::string                       defPos
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.look.groupTitle"_tr());
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
    form.sendTo(
        player,
        [this,
         group](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendGroupLookOperatorPage(player, group, elePos);
            }

            for (auto& perRes :
                 manager::CFSPManager::getInstance().groupLookAt(&player, group->mData.name, targetPos.value()))
                perRes.sendTo(player);
        }
    );
}

void GuiManager::sendGroupMoveOperatorPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    uint                              perm,
    std::string                       defPos,
    std::string                       defSpeed,
    int                               defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.move.groupTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)group::GroupPermission::MoveTo) op.emplace_back("gui.move.moveto"_tr());
    if (perm & (uint)group::GroupPermission::NavTo) op.emplace_back("gui.move.navto"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("targetpos", "gui.para.targetPos"_tr(), "0 0 0", defPos);
    form.appendInput("speed", "gui.para.speed"_tr(), "4.3", defSpeed);
    form.sendTo(
        player,
        [this,
         group,
         perm,
         op](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("speed");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleSpeed = std::get<std::string>(it->second);
            auto speed    = this->tryGetFloat(eleSpeed);

            it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            if (!targetPos.has_value() || !speed.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.posError"_tr()).sendTo(player);
                return this->sendGroupMoveOperatorPage(player, group, perm, elePos, eleSpeed, opIndex);
            }
            if (eleOp == "gui.move.moveto"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupMoveTo(&player, group->mData.name, targetPos.value(), speed.value()))
                    perRes.sendTo(player);
            else if (eleOp == "gui.move.navto"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupNavTo(&player, group->mData.name, targetPos.value(), speed.value()))
                    perRes.sendTo(player);
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendGroupInvOperatorPage(Player& player, std::shared_ptr<group::CFSPGroup> group, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.inv.group.title"_tr());
    if (perm)
        form.appendButton("gui.inv.group.invInfo"_tr(), [this, group, perm](Player& player) {
            std::string info = "";
            for (auto& perRes : manager::CFSPManager::getInstance().groupInvInfo(&player, group->mData.name))
                info += perRes.mInfo;
            ll::form::SimpleForm("gui.inv.group.invInfo"_tr(), info)
                .sendTo(player, [this, group, perm](Player& player, int, ll::form::FormCancelReason cancelReason) {
                    if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                        this->sendGroupInvOperatorPage(player, group, perm);
                });
        });
    if (perm & (uint)group::GroupPermission::Drop)
        form.appendButton("gui.inv.group.drop"_tr(), [this, group, perm](Player& player) {
            sendGroupDropPage(player, group, perm);
        });
    if (perm & (uint)group::GroupPermission::DropInv)
        form.appendButton("gui.inv.group.dropinv"_tr(), [this, group, perm](Player& player) {
            sendGroupDropInvPage(player, group, perm);
        });
    form.sendTo(player, [this, group](Player& player, int, ll::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
            this->sendOperateGroupPage(player, group);
    });
}

void GuiManager::sendGroupDropPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    uint                              perm,
    std::string                       defTimes,
    std::string                       defInterval
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.inv.group.drop"_tr());
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         group,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendGroupInvOperatorPage(player, group, perm);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!times.has_value() || !interval.has_value()) {
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                return this->sendGroupDropPage(player, group, perm, eleTimes, eleInterval);
            }

            for (auto& perRes : manager::CFSPManager::getInstance()
                                    .groupDrop(&player, group->mData.name, times.value(), interval.value()))
                perRes.sendTo(player);
        }
    );
}

void GuiManager::sendGroupDropInvPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    uint                              perm,
    std::string                       defTimes,
    std::string                       defInterval
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.inv.group.dropinv"_tr());
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         group,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendGroupInvOperatorPage(player, group, perm);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!times.has_value() || !interval.has_value()) {
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                return this->sendGroupDropInvPage(player, group, perm, eleTimes, eleInterval);
            }

            for (auto& perRes : manager::CFSPManager::getInstance()
                                    .groupDropInv(&player, group->mData.name, times.value(), interval.value()))
                perRes.sendTo(player);
        }
    );
}

void GuiManager::sendGroupMessageOperatorPage(Player& player, std::shared_ptr<group::CFSPGroup> group, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.message.groupTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)group::GroupPermission::Chat) op.emplace_back("gui.message.chat"_tr());
    if (perm & (uint)group::GroupPermission::RunCmd) op.emplace_back("gui.message.runcmd"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op);
    form.appendInput("text", "gui.para.text"_tr());
    form.sendTo(
        player,
        [this,
         group](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("text");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleText = std::get<std::string>(it->second);

            if (eleOp == "gui.message.chat"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance().groupChat(&player, group->mData.name, eleText))
                    perRes.sendTo(player);
            else if (eleOp == "gui.message.runcmd"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance().groupRunCmd(&player, group->mData.name, eleText))
                    perRes.sendTo(player);
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendGroupActionOperatorPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    uint                              perm,
    std::string                       defTimes,
    std::string                       defInterval,
    int                               defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.action.groupTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)group::GroupPermission::Attack) op.emplace_back("gui.action.attack"_tr());
    if (perm & (uint)group::GroupPermission::Build) op.emplace_back("gui.action.build"_tr());
    if (perm & (uint)group::GroupPermission::Interact) op.emplace_back("gui.action.interact"_tr());
    if (perm & (uint)group::GroupPermission::Jump) op.emplace_back("gui.action.jump"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         group,
         op,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!times.has_value() || !interval.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                return this->sendGroupActionOperatorPage(player, group, perm, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.action.attack"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupAttack(&player, group->mData.name, times.value(), interval.value()))
                    perRes.sendTo(player);
            else if (eleOp == "gui.action.build"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupBuild(&player, group->mData.name, times.value(), interval.value()))
                    perRes.sendTo(player);
            else if (eleOp == "gui.action.interact"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupInteract(&player, group->mData.name, times.value(), interval.value()))
                    perRes.sendTo(player);
            else if (eleOp == "gui.action.jump"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance()
                                        .groupJump(&player, group->mData.name, times.value(), interval.value()))
                    perRes.sendTo(player);
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendGroupLongActionOperatorPage(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    uint                              perm,
    std::string                       defLong,
    std::string                       defTimes,
    std::string                       defInterval,
    int                               defOp
) {
    using ll::i18n_literals::operator""_tr;
    auto                     form = ll::form::CustomForm("gui.longAction.groupTitle"_tr());
    std::vector<std::string> op;
    if (perm & (uint)group::GroupPermission::Use) op.emplace_back("gui.longAction.use"_tr());
    if (perm & (uint)group::GroupPermission::Destroy) op.emplace_back("gui.longAction.destroy"_tr());
    form.appendDropdown("operate", "gui.para.operate"_tr(), op, defOp);
    form.appendInput("long", "gui.para.long"_tr(), "10", defLong);
    form.appendInput("times", "gui.para.times"_tr(), "1", defTimes);
    form.appendInput("interval", "gui.para.interval"_tr(), "1", defInterval);
    form.sendTo(
        player,
        [this,
         group,
         op,
         perm](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("operate");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleOp = std::get<std::string>(it->second);

            it = elements.value().find("long");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleLong = std::get<std::string>(it->second);
            auto _long   = this->tryGetInt(eleLong);

            it = elements.value().find("times");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleTimes = std::get<std::string>(it->second);
            auto times    = this->tryGetInt(eleTimes);

            it = elements.value().find("interval");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto eleInterval = std::get<std::string>(it->second);
            auto interval    = this->tryGetInt(eleInterval);

            if (!_long.has_value() || !times.has_value() || !interval.has_value()) {
                int size = (int)op.size(), opIndex = -1;
                for (int i = 0; i < size; i++)
                    if (eleOp == op[i]) {
                        opIndex = i;
                        break;
                    }
                if (opIndex == -1) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
                base::OperateResult::error("gui.para.intError"_tr()).sendTo(player);
                return this
                    ->sendGroupLongActionOperatorPage(player, group, perm, eleLong, eleTimes, eleInterval, opIndex);
            }

            if (eleOp == "gui.longAction.use"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance()
                         .groupUse(&player, group->mData.name, _long.value(), times.value(), interval.value()))
                    perRes.sendTo(player);
            else if (eleOp == "gui.longAction.destroy"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance()
                         .groupDestroy(&player, group->mData.name, _long.value(), times.value(), interval.value()))
                    perRes.sendTo(player);
            else return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
        }
    );
}

void GuiManager::sendGroupStatusOperatorPage(Player& player, std::shared_ptr<group::CFSPGroup> group, uint perm) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.status.groupTitle"_tr());
    if (perm & (uint)group::GroupPermission::Sneaking) form.appendToggle("sneaking", "gui.status.sneaking"_tr(), false);
    if (perm & (uint)group::GroupPermission::Swimming) form.appendToggle("swimming", "gui.status.swimming"_tr(), false);
    if (perm & (uint)group::GroupPermission::Flying) form.appendToggle("flying", "gui.status.flying"_tr(), false);
    if (perm & (uint)group::GroupPermission::Sprinting)
        form.appendToggle("sprinting", "gui.status.sprinting"_tr(), false);
    form.sendTo(
        player,
        [this,
         group](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            if (auto it = elements.value().find("sneaking");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSneaking(&player, group->mData.name, true))
                        perRes.sendTo(player);
                } else
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSneaking(&player, group->mData.name, false))
                        perRes.sendTo(player);
            }

            if (auto it = elements.value().find("swimming");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSwimming(&player, group->mData.name, true))
                        perRes.sendTo(player);
                } else
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSwimming(&player, group->mData.name, false))
                        perRes.sendTo(player);
            }

            if (auto it = elements.value().find("flying");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupFlying(&player, group->mData.name, true))
                        perRes.sendTo(player);
                } else
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupFlying(&player, group->mData.name, false))
                        perRes.sendTo(player);
            }

            if (auto it = elements.value().find("sprinting");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second)) {
                if ((bool)std::get<uint64>(it->second)) {
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSprinting(&player, group->mData.name, true))
                        perRes.sendTo(player);
                } else
                    for (auto& perRes :
                         manager::CFSPManager::getInstance().groupSprinting(&player, group->mData.name, false))
                        perRes.sendTo(player);
            }
        }
    );
}

void GuiManager::sendGroupPermPage(Player& player, std::shared_ptr<group::CFSPGroup> group) {
    using ll::i18n_literals::operator""_tr;
    std::vector<std::pair<std::string, std::string>> splist;
    auto                                             level = ll::service::getLevel();
    level->forEachPlayer([&splist, pname = player.mName.get()](Player& player) {
        if (!player.isSimulatedPlayer() && pname != player.mName.get())
            splist.emplace_back(std::make_pair(player.mName, player.getUuid().asString()));
        return true;
    });
    for (auto i : group->mData.permission)
        splist.emplace_back(std::make_pair(base::utils::tryGetPlayerName(i.first), i.first));
    std::sort(splist.begin(), splist.end());

    auto form = ll::form::SimpleForm("gui.perm.groupTitle"_tr());
    int  size = (int)splist.size();
    if (size) {
        form.appendButton(splist[0].first, [this, group, targetPlayer = splist[0]](Player& player) {
            this->sendGroupPermPage2(player, group, targetPlayer.first, targetPlayer.second);
        });
        for (int i = 1; i < size; i++)
            if (splist[i] != splist[i - 1])
                form.appendButton(splist[i].first, [this, group, targetPlayer = splist[i]](Player& player) {
                    this->sendGroupPermPage2(player, group, targetPlayer.first, targetPlayer.second);
                });
    }
    form.appendButton("gui.perm.publicGroup"_tr(), [this, group](Player& player) {
        this->sendGroupPermPage2(player, group);
    });
    form.sendTo(player, [this, group](Player& player, int, ll::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
            this->sendOperateGroupPage(player, group);
    });
}

void GuiManager::sendGroupPermPage2(
    Player&                           player,
    std::shared_ptr<group::CFSPGroup> group,
    std::string                       targetPlayerName,
    std::string                       targetPlayerUuid
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.perm.groupTitle2"_tr(
        group->mData.name,
        targetPlayerName != "" ? targetPlayerName : "gui.perm.publicGroup"_tr()
    ));

    uint perm = 0;
    if (targetPlayerName == "") perm = group->mData.publicPermission;
    else {
        auto it = group->mData.permission.find(targetPlayerUuid);
        if (it != group->mData.permission.end()) perm = it->second;
    }

    form.appendToggle("addSp", "gui.perm.addSp"_tr(), perm & (uint)group::GroupPermission::AddSp);
    form.appendToggle("rmSp", "gui.perm.rmSp"_tr(), perm & (uint)group::GroupPermission::RmSp);
    form.appendToggle("delete", "gui.perm.delete"_tr(), perm & (uint)group::GroupPermission::Delete);
    form.appendToggle("spawn", "gui.perm.spawn"_tr(), perm & (uint)group::GroupPermission::Spawn);
    form.appendToggle("despawn", "gui.perm.despawn"_tr(), perm & (uint)group::GroupPermission::Despawn);
    form.appendToggle("respawn", "gui.perm.respawn"_tr(), perm & (uint)group::GroupPermission::Respawn);
    form.appendToggle("deleteSp", "gui.perm.deleteSp"_tr(), perm & (uint)group::GroupPermission::DeleteSp);
    form.appendToggle("stop", "gui.perm.stop"_tr(), perm & (uint)group::GroupPermission::Stop);
    form.appendToggle("drop", "gui.perm.drop"_tr(), perm & (uint)group::GroupPermission::Drop);
    form.appendToggle("dropInv", "gui.perm.dropInv"_tr(), perm & (uint)group::GroupPermission::DropInv);
    form.appendToggle("sneaking", "gui.perm.sneaking"_tr(), perm & (uint)group::GroupPermission::Sneaking);
    form.appendToggle("dwimming", "gui.perm.swimming"_tr(), perm & (uint)group::GroupPermission::Swimming);
    form.appendToggle("flying", "gui.perm.flying"_tr(), perm & (uint)group::GroupPermission::Flying);
    form.appendToggle("sprinting", "gui.perm.sprinting"_tr(), perm & (uint)group::GroupPermission::Sprinting);
    form.appendToggle("attack", "gui.perm.attack"_tr(), perm & (uint)group::GroupPermission::Attack);
    form.appendToggle("build", "gui.perm.build"_tr(), perm & (uint)group::GroupPermission::Build);
    form.appendToggle("interact", "gui.perm.interact"_tr(), perm & (uint)group::GroupPermission::Interact);
    form.appendToggle("jump", "gui.perm.jump"_tr(), perm & (uint)group::GroupPermission::Jump);
    form.appendToggle("use", "gui.perm.use"_tr(), perm & (uint)group::GroupPermission::Use);
    form.appendToggle("destroy", "gui.perm.destroy"_tr(), perm & (uint)group::GroupPermission::Destroy);
    form.appendToggle("chat", "gui.perm.chat"_tr(), perm & (uint)group::GroupPermission::Chat);
    form.appendToggle("runCmd", "gui.perm.runCmd"_tr(), perm & (uint)group::GroupPermission::RunCmd);
    form.appendToggle("lookAt", "gui.perm.lookAt"_tr(), perm & (uint)group::GroupPermission::LookAt);
    form.appendToggle("moveTo", "gui.perm.moveTo"_tr(), perm & (uint)group::GroupPermission::MoveTo);
    form.appendToggle("navTo", "gui.perm.navTo"_tr(), perm & (uint)group::GroupPermission::NavTo);
    form.appendToggle("tp", "gui.perm.tp"_tr(), perm & (uint)group::GroupPermission::Tp);
    form.appendToggle("select", "gui.perm.select"_tr(), perm & (uint)group::GroupPermission::Select);

    form.sendTo(
        player,
        [this, group, targetPlayerUuid](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendGroupPermPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            uint newPerm = 0;

            if (auto it = elements.value().find("addSp");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::AddSp;

            if (auto it = elements.value().find("rmSp");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::RmSp;

            if (auto it = elements.value().find("delete");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Delete;

            if (auto it = elements.value().find("spawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Spawn;

            if (auto it = elements.value().find("despawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Despawn;

            if (auto it = elements.value().find("respawn");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Respawn;

            if (auto it = elements.value().find("deleteSp");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::DeleteSp;

            if (auto it = elements.value().find("stop");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Stop;

            if (auto it = elements.value().find("drop");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Drop;

            if (auto it = elements.value().find("dropInv");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::DropInv;

            if (auto it = elements.value().find("sneaking");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Sneaking;

            if (auto it = elements.value().find("dwimming");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Swimming;

            if (auto it = elements.value().find("flying");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Flying;

            if (auto it = elements.value().find("sprinting");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Sprinting;

            if (auto it = elements.value().find("attack");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Attack;

            if (auto it = elements.value().find("build");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Build;

            if (auto it = elements.value().find("interact");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Interact;

            if (auto it = elements.value().find("jump");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Jump;

            if (auto it = elements.value().find("use");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Use;

            if (auto it = elements.value().find("destroy");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Destroy;

            if (auto it = elements.value().find("chat");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Chat;

            if (auto it = elements.value().find("runCmd");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::RunCmd;

            if (auto it = elements.value().find("lookAt");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::LookAt;

            if (auto it = elements.value().find("moveTo");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::MoveTo;

            if (auto it = elements.value().find("navTo");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::NavTo;

            if (auto it = elements.value().find("tp");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Tp;

            if (auto it = elements.value().find("select");
                it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                if ((bool)std::get<uint64>(it->second)) newPerm |= (uint)group::GroupPermission::Select;

            if (targetPlayerUuid == "") group->mData.publicPermission = newPerm;
            else if (newPerm) group->mData.permission[targetPlayerUuid] = newPerm;
            else if (auto it = group->mData.permission.find(targetPlayerUuid); it != group->mData.permission.end())
                group->mData.permission.erase(it);
            group->save();

            base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
        }
    );
}
} // namespace coral_fans::cfsp::gui