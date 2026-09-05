#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupLookPage : public GuiPage {
public:
    GroupLookPage(std::shared_ptr<group::CFSPGroup> group, std::shared_ptr<GuiPage> parent, std::string defPos = "")
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mDefPos(std::move(defPos)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    std::string                       mDefPos;
};

} // namespace coral_fans::cfsp::gui
