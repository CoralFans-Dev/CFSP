#include "ManagerMainMenuPage.h"
#include "AllGroupListPage.h"
#include "AllSpListPage.h"
#include "ManagePage.h"
#include "PublicSpListPage.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void ManagerMainMenuPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.mainmenu.title"_tr());
    form.appendButton("gui.managermainmenu.splistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<AllSpListPage>(self)->sendTo(player);
    });
    form.appendButton("gui.mainmenu.publicSplistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<PublicSpListPage>(self)->sendTo(player);
    });
    form.appendButton("gui.managermainmenu.grouplistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<AllGroupListPage>(self)->sendTo(player);
    });
    form.appendButton("gui.managermainmenu.managerlistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<ManagePage>(self)->sendTo(player);
    });
    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
