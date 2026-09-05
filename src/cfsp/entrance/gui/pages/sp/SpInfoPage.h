#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>

namespace coral_fans::cfsp::gui {

class SpInfoPage : public GuiPage {
public:
    SpInfoPage(std::shared_ptr<simulated_player::SimPlayer> cfsp, std::shared_ptr<GuiPage> parent)
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
};

} // namespace coral_fans::cfsp::gui
