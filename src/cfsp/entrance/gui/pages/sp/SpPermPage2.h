#pragma once

#include "cfsp/core/simPlayer/SimPlayer.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class SpPermPage2 : public GuiPage {
public:
    SpPermPage2(
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        std::shared_ptr<GuiPage>                     parent,
        std::string                                  targetPlayerName = "",
        std::string                                  targetPlayerUuid = ""
    )
    : GuiPage(std::move(parent)),
      mCfsp(std::move(cfsp)),
      mTargetPlayerName(std::move(targetPlayerName)),
      mTargetPlayerUuid(std::move(targetPlayerUuid)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<simulated_player::SimPlayer> mCfsp;
    std::string                                  mTargetPlayerName;
    std::string                                  mTargetPlayerUuid;
};

} // namespace coral_fans::cfsp::gui
