#pragma once

#include "cfsp/entrance/gui/base/GuiPage.h"
#include <string>
#include <unordered_set>

namespace coral_fans::cfsp::gui {

class GroupDeleteSpConfirmPage : public GuiPage {
public:
    explicit GroupDeleteSpConfirmPage(std::unordered_set<std::string> confirmList)
    : GuiPage(nullptr),
      mConfirmList(std::move(confirmList)) {}

public:
    void sendTo(Player& player) override;

private:
    std::unordered_set<std::string> mConfirmList;
};

} // namespace coral_fans::cfsp::gui
