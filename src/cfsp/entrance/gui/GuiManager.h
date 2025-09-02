#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "mc/world/actor/player/Player.h"
#include <optional>

namespace coral_fans::cfsp::gui {
class GuiManager {
public:
    static GuiManager& getInstance();

private:
    std::optional<Vec3> tryGetVec3(std::string);

private:
    void sendPublicSplist(Player&);
    void sendAllSplist(Player&);
    void sendAllGroupList(Player&);
    void sendManagePage(Player&);
    void sendNewSpPage(Player& player, int defDim = 0, std::string defPos = "", std::string defName = "");
    void sendCreateGroupPage(Player&);

public:
    void sendMainMenu(Player&);
    void sendManagerMainMenu(Player&);
    void sendSplist(Player&);
    void sendGroupList(Player&);
    void sendOperatorSpPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendOperatorGroupPage(Player&, std::shared_ptr<group::CFSPGroup>);
};
} // namespace coral_fans::cfsp::gui