#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>

namespace coral_fans::cfsp::gui {

class GroupStatusPage : public GuiPage {
public:
    GroupStatusPage(std::shared_ptr<group::CFSPGroup> group, uint perm, std::shared_ptr<GuiPage> parent)
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mPerm(perm) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    uint                              mPerm;
};

} // namespace coral_fans::cfsp::gui
