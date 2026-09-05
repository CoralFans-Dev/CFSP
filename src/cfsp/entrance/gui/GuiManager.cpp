#include "GuiManager.h"
#include "cfsp/entrance/gui/pages/menu/GroupListPage.h"
#include "cfsp/entrance/gui/pages/menu/MainMenuPage.h"
#include "cfsp/entrance/gui/pages/menu/ManagerMainMenuPage.h"
#include "cfsp/entrance/gui/pages/menu/SpListPage.h"
#include <memory>

namespace coral_fans::cfsp::gui {

GuiManager& GuiManager::getInstance() {
    static GuiManager instance;
    return instance;
}

void GuiManager::sendMainMenu(Player& player) { std::make_shared<MainMenuPage>()->sendTo(player); }

void GuiManager::sendManagerMainMenu(Player& player) { std::make_shared<ManagerMainMenuPage>()->sendTo(player); }

void GuiManager::sendSplist(Player& player) { std::make_shared<SpListPage>()->sendTo(player); }

void GuiManager::sendGroupList(Player& player) { std::make_shared<GroupListPage>()->sendTo(player); }

} // namespace coral_fans::cfsp::gui
