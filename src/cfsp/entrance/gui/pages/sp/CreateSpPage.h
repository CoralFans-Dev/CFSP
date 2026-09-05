#pragma once

#include "cfsp/entrance/gui/base/GuiPage.h"
#include <string>

namespace coral_fans::cfsp::gui {

class CreateSpPage : public GuiPage {
public:
    CreateSpPage(
        std::shared_ptr<GuiPage> parent,
        int                      defDim          = 0,
        std::string              defPos          = "",
        std::string              defName         = "",
        int                      defLockUniqueId = 0
    )
    : GuiPage(std::move(parent)),
      mDefDim(defDim),
      mDefPos(std::move(defPos)),
      mDefName(std::move(defName)),
      mDefLockUniqueId(defLockUniqueId) {}

public:
    void sendTo(Player& player) override;

private:
    int         mDefDim;
    std::string mDefPos;
    std::string mDefName;
    int         mDefLockUniqueId;
};

} // namespace coral_fans::cfsp::gui
