#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupMovePage : public GuiPage {
public:
    GroupMovePage(
        std::shared_ptr<group::CFSPGroup> group,
        uint                              perm,
        std::shared_ptr<GuiPage>          parent,
        std::string                       defPos   = "",
        std::string                       defSpeed = "4.3",
        int                               defOp    = 0
    )
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mPerm(perm),
      mDefPos(std::move(defPos)),
      mDefSpeed(std::move(defSpeed)),
      mDefOp(defOp) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    uint                              mPerm;
    std::string                       mDefPos;
    std::string                       mDefSpeed;
    int                               mDefOp;
};

} // namespace coral_fans::cfsp::gui
