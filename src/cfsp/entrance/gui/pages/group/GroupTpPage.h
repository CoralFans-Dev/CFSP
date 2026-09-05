#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupTpPage : public GuiPage {
public:
    GroupTpPage(
        std::shared_ptr<group::CFSPGroup> group,
        std::shared_ptr<GuiPage>          parent,
        int                               defDim = 0,
        std::string                       defPos = ""
    )
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mDefDim(defDim),
      mDefPos(std::move(defPos)) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    int                               mDefDim;
    std::string                       mDefPos;
};

} // namespace coral_fans::cfsp::gui
