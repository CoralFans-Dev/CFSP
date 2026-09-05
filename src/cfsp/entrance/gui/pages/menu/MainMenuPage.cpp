#include "MainMenuPage.h"
#include "GroupListPage.h"
#include "PublicSpListPage.h"
#include "SpListPage.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::gui {

void MainMenuPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::SimpleForm("gui.mainmenu.title"_tr());
    form.appendButton("gui.mainmenu.splistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<SpListPage>(self)->sendTo(player);
    });
    form.appendButton("gui.mainmenu.publicSplistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<PublicSpListPage>(self)->sendTo(player);
    });
    form.appendButton("gui.mainmenu.grouplistButton"_tr(), [self = shared_from_this()](Player& player) {
        std::make_shared<GroupListPage>(self)->sendTo(player);
    });
    form.sendTo(player);
}

} // namespace coral_fans::cfsp::gui
