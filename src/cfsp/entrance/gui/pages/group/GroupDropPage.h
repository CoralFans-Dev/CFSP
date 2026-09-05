#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupDropPage : public GuiPage {
public:
    GroupDropPage(
        std::shared_ptr<group::CFSPGroup> group,
        std::shared_ptr<GuiPage>          parent,
        std::string                       defTimes    = "1",
        std::string                       defInterval = "1"
    )
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mDefTimes(std::move(defTimes)),
      mDefInterval(std::move(defInterval)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    std::string                       mDefTimes;
    std::string                       mDefInterval;
};

} // namespace coral_fans::cfsp::gui
