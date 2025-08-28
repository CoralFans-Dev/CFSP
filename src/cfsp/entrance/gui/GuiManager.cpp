#include "GuiManager.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {
GuiManager& GuiManager::getInstance() {
    static GuiManager instance;
    return instance;
}
void GuiManager::sendMainMenu(Player*) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.mainmenu.title"_tr());
    form.appendButton("gui.mainmenu.splistButton"_tr(), [this](Player& player) { this->sendSplist(&player); });
    form.appendButton("gui.mainmenu.publicSplistButton"_tr(), [this](Player& player) { this->sendSplist(&player); });
    form.appendButton("gui.mainmenu.grouplistButton"_tr(), [this](Player& player) { this->sendGroupList(&player); });
}
} // namespace coral_fans::cfsp::gui