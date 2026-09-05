#pragma once

#include "cfsp/entrance/gui/base/GuiPage.h"

namespace coral_fans::cfsp::gui {

class ManagerMainMenuPage : public GuiPage {
public:
    ManagerMainMenuPage() : GuiPage(nullptr) {}

public:
    void sendTo(Player& player) override;
};

} // namespace coral_fans::cfsp::gui
