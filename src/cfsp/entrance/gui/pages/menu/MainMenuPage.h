#pragma once

#include "cfsp/entrance/gui/base/GuiPage.h"

namespace coral_fans::cfsp::gui {

class MainMenuPage : public GuiPage {
public:
    MainMenuPage() : GuiPage(nullptr) {}

public:
    void sendTo(Player& player) override;
};

} // namespace coral_fans::cfsp::gui
