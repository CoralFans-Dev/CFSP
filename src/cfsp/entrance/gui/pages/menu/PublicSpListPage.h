#pragma once

#include "cfsp/entrance/gui/base/GuiPage.h"

namespace coral_fans::cfsp::gui {

class PublicSpListPage : public GuiPage {
public:
    using GuiPage::GuiPage;

public:
    void sendTo(Player& player) override;
};

} // namespace coral_fans::cfsp::gui
