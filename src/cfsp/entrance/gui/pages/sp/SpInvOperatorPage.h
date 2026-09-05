#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>

namespace coral_fans::cfsp::gui {

class SpInvOperatorPage : public GuiPage {
public:
    SpInvOperatorPage(std::shared_ptr<simulated_player::SimPlayer> cfsp, uint perm, std::shared_ptr<GuiPage> parent)
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mPerm(perm) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    uint                                         mPerm;
};

} // namespace coral_fans::cfsp::gui
