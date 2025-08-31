#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
#include <string>


namespace coral_fans::cfsp::gui {
GuiManager& GuiManager::getInstance() {
    static GuiManager instance;
    return instance;
}

std::optional<Vec3> GuiManager::tryGetVec3(std::string str) {
    std::istringstream iss(str);
    float              x, y, z;
    char               test;
    if (!(iss >> x >> y >> z) || iss >> test) return std::nullopt;
    return Vec3(x, y, z);
}

void GuiManager::sendMainMenu(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.mainmenu.title"_tr());
    form.appendButton("gui.mainmenu.splistButton"_tr(), [this](Player& player) { this->sendSplist(player); });
    form.appendButton("gui.mainmenu.publicSplistButton"_tr(), [this](Player& player) {
        this->sendPublicSplist(player);
    });
    form.appendButton("gui.mainmenu.grouplistButton"_tr(), [this](Player& player) { this->sendGroupList(player); });
    form.sendTo(player);
}

void GuiManager::sendManagerMainMenu(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.mainmenu.title"_tr());
    form.appendButton("gui.managermainmenu.splistButton"_tr(), [this](Player& player) { this->sendAllSplist(player); });
    form.appendButton("gui.managermainmenu.grouplistButton"_tr(), [this](Player& player) {
        this->sendAllGroupList(player);
    });
    form.appendButton("gui.managermainmenu.managerlistButton"_tr(), [this](Player& player) {
        this->sendManagePage(player);
    });
    form.sendTo(player);
}

void GuiManager::sendSplist(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.splist.title"_tr());
    for (auto spn : manager::CFSPManager::getInstance().getSpNamesSorted(&player)) {
        form.appendButton(spn, [spn, this](Player& player) {
            auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(spn);
            if (!cfsp.has_value()) return base::OperateResult::error("command.fail.spNoFind"_tr()).sendTo(player);
            sendOperatorSpPage(player, cfsp.value());
        });
    }
    if (manager::CFSPManager::getInstance().canCreatePlayer(&player)) {
        form.appendButton("gui.splist.newsp"_tr(), [this](Player& player) {
            auto pos = player.getFeetPos();
            this->sendNewSpPage(
                player,
                player.getDimensionId(),
                std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
            );
        });
    }
    form.sendTo(player);
}

void GuiManager::sendGroupList(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.grouplist.title"_tr());
    for (auto groupName : manager::CFSPManager::getInstance().getGroupNamesSorted(&player)) {
        form.appendButton(groupName, [groupName, this](Player& player) {
            auto group = manager::CFSPManager::getInstance().tryGetCFSPGroup(groupName);
            if (!group.has_value()) return base::OperateResult::error("command.fail.groupNoFind"_tr()).sendTo(player);
            sendOperatorGroupPage(player, group.value());
        });
    }
    if (manager::CFSPManager::getInstance().canCreateGroup(&player)) {
        form.appendButton("gui.grouplist.newgroup"_tr(), [this](Player& player) { this->sendCreateGroupPage(player); });
    }
}

void GuiManager::sendPublicSplist(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.publicSpList.title"_tr());
    for (auto spn : manager::CFSPManager::getInstance().getPublicSpNamesSorted()) {
        form.appendButton(spn, [spn, this](Player& player) {
            auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(spn);
            if (!cfsp.has_value()) return base::OperateResult::error("command.fail.spNoFind"_tr()).sendTo(player);
            sendOperatorSpPage(player, cfsp.value());
        });
    }
    form.sendTo(player);
}

void GuiManager::sendAllSplist(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.managerSpList.title"_tr());
    for (auto spn : manager::CFSPManager::getInstance().getAllSpNamesSorted()) {
        form.appendButton(spn, [spn, this](Player& player) {
            auto cfsp = manager::CFSPManager::getInstance().tryGetCFSP(spn);
            if (!cfsp.has_value()) return base::OperateResult::error("command.fail.spNoFind"_tr()).sendTo(player);
            sendOperatorSpPage(player, cfsp.value());
        });
    }
    form.appendButton("gui.managerSpList.newsp"_tr(), [this](Player& player) {
        auto pos = player.getFeetPos();
        this->sendNewSpPage(
            player,
            player.getDimensionId(),
            std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)
        );
    });
    form.sendTo(player);
}

void GuiManager::sendAllGroupList(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.grouplist.title"_tr());
    for (auto groupName : manager::CFSPManager::getInstance().getAllGroupNamesSorted()) {
        form.appendButton(groupName, [groupName, this](Player& player) {
            auto group = manager::CFSPManager::getInstance().tryGetCFSPGroup(groupName);
            if (!group.has_value()) return base::OperateResult::error("command.fail.groupNoFind"_tr()).sendTo(player);
            sendOperatorGroupPage(player, group.value());
        });
    }
    form.appendButton("gui.grouplist.newgroup"_tr(), [this](Player& player) { this->sendCreateGroupPage(player); });
    form.sendTo(player);
}

void GuiManager::sendManagePage(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.manage.title"_tr());
    form.appendToggle("autojoin", "gui.manage.autojoin"_tr(), manager::CFSPManager::getInstance().getAutoJoin());
    form.appendToggle(
        "autorespawn",
        "gui.manage.autorespawn"_tr(),
        manager::CFSPManager::getInstance().getAutoRespawn()
    );
    form.appendToggle(
        "autodespawn",
        "gui.manage.autodespawn"_tr(),
        manager::CFSPManager::getInstance().getAutoDespawn()
    );
    form.sendTo(
        player,
        [](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (!cancelReason.has_value() && elements.has_value()
                && manager::CFSPManager::getInstance().isManager(&player)) {
                auto it = elements.value().find("autojoin");
                if (it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                    manager::CFSPManager::getInstance().setAutoJoin(std::get<uint64>(it->second));
                it = elements.value().find("autorespawn");
                if (it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                    manager::CFSPManager::getInstance().setAutoRespawn(std::get<uint64>(it->second));
                it = elements.value().find("autodespawn");
                if (it != elements.value().end() && std::holds_alternative<uint64>(it->second))
                    manager::CFSPManager::getInstance().setAutoDespawn(std::get<uint64>(it->second));
                base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
            }
        }
    );
}
} // namespace coral_fans::cfsp::gui