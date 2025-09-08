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
    std::optional<Vec3> tryGetVec3(std::string&);

private:
    void sendOperateSpPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendPublicSplist(Player&);
    void sendAllSplist(Player&);
    void sendAllGroupList(Player&);
    void sendManagePage(Player&);
    void sendNewSpPage(Player& player, int defDim = 0, std::string defPos = "", std::string defName = "");
    void sendSpInfoPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendSpInvOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpTpOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        int                                          defDim = 0,
        std::string                                  defPos = ""
    );
    void sendSpMessageOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpMoveOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpLookOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendSpActionOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpLongActionOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpStatusOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);

private:
    void sendOperateGroupPage(Player&, std::shared_ptr<group::CFSPGroup>);
    void sendCreateGroupPage(Player&);

public:
    void sendMainMenu(Player&);
    void sendManagerMainMenu(Player&);
    void sendSplist(Player&);
    void sendGroupList(Player&);
};
} // namespace coral_fans::cfsp::gui