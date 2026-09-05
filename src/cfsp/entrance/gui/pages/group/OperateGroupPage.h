#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>

namespace coral_fans::cfsp::gui {

class OperateGroupPage : public GuiPage {
public:
    OperateGroupPage(std::shared_ptr<group::CFSPGroup> group, std::shared_ptr<GuiPage> parent = nullptr)
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
};

} // namespace coral_fans::cfsp::gui
