#pragma once

#include "mc/world/actor/player/Player.h"

namespace coral_fans::cfsp::gui {
class GuiManager {
public:
    static GuiManager& getInstance();

public:
    void sendMainMenu(Player&);
    void sendManagerMainMenu(Player&);
    void sendSplist(Player&);
    void sendGroupList(Player&);
};
} // namespace coral_fans::cfsp::gui
