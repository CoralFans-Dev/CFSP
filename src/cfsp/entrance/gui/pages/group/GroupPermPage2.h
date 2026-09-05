#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupPermPage2 : public GuiPage {
public:
    GroupPermPage2(
        std::shared_ptr<group::CFSPGroup> group,
        std::shared_ptr<GuiPage>          parent,
        std::string                       targetPlayerName = "",
        std::string                       targetPlayerUuid = ""
    )
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mTargetPlayerName(std::move(targetPlayerName)),
      mTargetPlayerUuid(std::move(targetPlayerUuid)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    std::string                       mTargetPlayerName;
    std::string                       mTargetPlayerUuid;
};

} // namespace coral_fans::cfsp::gui
