#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/phys/HitResult.h"
#include <vector>

namespace coral_fans::cfsp::gui {
void GuiManager::sendOperateGroupPage(Player& player, std::shared_ptr<group::CFSPGroup> group) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm(group->mData.name);
    uint perm;
    if (manager::CFSPManager::getInstance().isManager(&player))
        perm = manager::CFSPManager::getInstance().getGroupPermissionMask();
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
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
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
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
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
    form.appendInput("targetpos", "gui.para.targetpos"_tr(), "0 0 0", defPos);
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
        form.appendButton("gui.inv.group.drop"_tr(), [gname = group->mData.name](Player& player) {
            for (auto& perRes : manager::CFSPManager::getInstance().groupDrop(&player, gname)) perRes.sendTo(player);
        });
    if (perm & (uint)group::GroupPermission::DropInv)
        form.appendButton("gui.inv.group.dropinv"_tr(), [gname = group->mData.name](Player& player) {
            for (auto& perRes : manager::CFSPManager::getInstance().groupDropInv(&player, gname)) perRes.sendTo(player);
        });
    form.sendTo(player, [this, group](Player& player, int, ll::form::FormCancelReason cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
            this->sendOperateGroupPage(player, group);
    });
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
    if (perm & (uint)group::GroupPermission::Sneaking)
        form.appendDropdown(
            "sneaking",
            "gui.status.sneaking"_tr(),
            std::vector<std::string>{"gui.status.sneak"_tr(), "gui.status.releasesneak"_tr()}
        );
    if (perm & (uint)group::GroupPermission::Swimming)
        form.appendDropdown(
            "swimming",
            "gui.status.swimming"_tr(),
            std::vector<std::string>{"gui.status.swim"_tr(), "gui.status.releaseswim"_tr()}
        );
    if (perm & (uint)group::GroupPermission::Flying)
        form.appendDropdown(
            "flying",
            "gui.status.flying"_tr(),
            std::vector<std::string>{"gui.status.fly"_tr(), "gui.status.releasefly"_tr()}
        );
    if (perm & (uint)group::GroupPermission::Sprinting)
        form.appendDropdown(
            "sprinting",
            "gui.status.sprinting"_tr(),
            std::vector<std::string>{"gui.status.sprint"_tr(), "gui.status.releasesprint"_tr()}
        );
    form.sendTo(
        player,
        [this,
         group](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed)
                return this->sendOperateGroupPage(player, group);
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("sneaking");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto sneaking = std::get<std::string>(it->second);

            it = elements.value().find("swimming");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto swimming = std::get<std::string>(it->second);

            it = elements.value().find("flying");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto flying = std::get<std::string>(it->second);

            it = elements.value().find("sprinting");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);
            auto sprinting = std::get<std::string>(it->second);

            if (sneaking == "gui.status.sneak"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance().groupSneaking(&player, group->mData.name, true))
                    perRes.sendTo(player);
            else if (sneaking == "gui.status.releasesneak"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance().groupSneaking(&player, group->mData.name, false))
                    perRes.sendTo(player);

            if (swimming == "gui.status.swim"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance().groupSwimming(&player, group->mData.name, true))
                    perRes.sendTo(player);
            else if (swimming == "gui.status.releaseswim"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance().groupSwimming(&player, group->mData.name, false))
                    perRes.sendTo(player);

            if (flying == "gui.status.fly"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance().groupFlying(&player, group->mData.name, true))
                    perRes.sendTo(player);
            else if (flying == "gui.status.releasefly"_tr())
                for (auto& perRes : manager::CFSPManager::getInstance().groupFlying(&player, group->mData.name, false))
                    perRes.sendTo(player);

            if (sprinting == "gui.status.sprint"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance().groupSprinting(&player, group->mData.name, true))
                    perRes.sendTo(player);
            else if (sprinting == "gui.status.releasesprint"_tr())
                for (auto& perRes :
                     manager::CFSPManager::getInstance().groupSprinting(&player, group->mData.name, false))
                    perRes.sendTo(player);
        }
    );
}
} // namespace coral_fans::cfsp::gui