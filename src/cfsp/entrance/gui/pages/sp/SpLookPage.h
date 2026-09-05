#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpLookPage : public GuiPage {
public:
    SpLookPage(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        std::shared_ptr<GuiPage>                     parent,
        std::string                                  defPos = ""
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mDefPos(std::move(defPos)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    std::string                                  mDefPos;
};

} // namespace coral_fans::cfsp::gui
