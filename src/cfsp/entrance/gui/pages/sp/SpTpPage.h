#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpTpPage : public GuiPage {
public:
    SpTpPage(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        std::shared_ptr<GuiPage>                     parent,
        int                                          defDim = 0,
        std::string                                  defPos = ""
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mDefDim(defDim),
      mDefPos(std::move(defPos)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    int                                          mDefDim;
    std::string                                  mDefPos;
};

} // namespace coral_fans::cfsp::gui
