#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpDropPage : public GuiPage {
public:
    SpDropPage(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        std::shared_ptr<GuiPage>                     parent,
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1"
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mDefTimes(std::move(defTimes)),
      mDefInterval(std::move(defInterval)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    std::string                                  mDefTimes;
    std::string                                  mDefInterval;
};

} // namespace coral_fans::cfsp::gui
