#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/entrance/gui/base/GuiPage.h"
#include <memory>
#include <string>

namespace coral_fans::cfsp::gui {

class GroupLongActionPage : public GuiPage {
public:
    GroupLongActionPage(
        std::shared_ptr<group::CFSPGroup> group,
        uint                              perm,
        std::shared_ptr<GuiPage>          parent,
        std::string                       defLong     = "10",
        std::string                       defTimes    = "1",
        std::string                       defInterval = "1",
        int                               defOp       = 0
    )
    : GuiPage(std::move(parent)),
      mGroup(std::move(group)),
      mPerm(perm),
      mDefLong(std::move(defLong)),
      mDefTimes(std::move(defTimes)),
      mDefInterval(std::move(defInterval)),
      mDefOp(defOp) {}

public:
    void sendTo(Player& player) override;

private:
    std::shared_ptr<group::CFSPGroup> mGroup;
    uint                              mPerm;
    std::string                       mDefLong;
    std::string                       mDefTimes;
    std::string                       mDefInterval;
    int                               mDefOp;
};

} // namespace coral_fans::cfsp::gui
