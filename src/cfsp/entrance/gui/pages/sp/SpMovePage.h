#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpMovePage : public GuiPage {
public:
    SpMovePage(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::shared_ptr<GuiPage>                     parent,
        std::string                                  defPos   = "",
        std::string                                  defSpeed = "4.3",
        int                                          defOp    = 0
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mPerm(perm),
      mDefPos(std::move(defPos)),
      mDefSpeed(std::move(defSpeed)),
      mDefOp(defOp) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    uint                                         mPerm;
    std::string                                  mDefPos;
    std::string                                  mDefSpeed;
    int                                          mDefOp;
};

} // namespace coral_fans::cfsp::gui
