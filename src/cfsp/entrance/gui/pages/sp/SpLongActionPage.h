#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpLongActionPage : public GuiPage {
public:
    SpLongActionPage(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::shared_ptr<GuiPage>                     parent,
        std::string                                  defLong     = "10",
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1",
        int                                          defOp       = 0
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mPerm(perm),
      mDefLong(std::move(defLong)),
      mDefTimes(std::move(defTimes)),
      mDefInterval(std::move(defInterval)),
      mDefOp(defOp) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    uint                                         mPerm;
    std::string                                  mDefLong;
    std::string                                  mDefTimes;
    std::string                                  mDefInterval;
    int                                          mDefOp;
};

} // namespace coral_fans::cfsp::gui
