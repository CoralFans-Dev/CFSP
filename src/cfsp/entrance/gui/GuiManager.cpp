#include "GuiManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include <string>


namespace coral_fans::cfsp::gui {
GuiManager& GuiManager::getInstance() {
    static GuiManager instance;
    return instance;
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
    form.appendButton("gui.managermainmenu.managerlistbutton"_tr(), [this](Player& player) {
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
            if (!cfsp.has_value()) base::OperateResult::error("command.fail.spNoFind"_tr()).sendTo(player);
            sendSpOperatorList(player, cfsp.value());
        });
    }
    if (manager::CFSPManager::getInstance().canCreatePlayer(&player).mType == base::OperateResult::Type::success) {
        form.appendButton("gui.splist.newsp", [this](Player& player) {
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

} // namespace coral_fans::cfsp::gui