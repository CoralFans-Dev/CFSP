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
    std::optional<int>  tryGetInt(std::string&);

private:
    void sendOperateSpPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendPublicSplist(Player&);
    void sendAllSplist(Player&);
    void sendAllGroupList(Player&);
    void sendManagePage(Player&);
    void sendCreateSpPage(Player& player, int defDim = 0, std::string defPos = "", std::string defName = "");
    void sendSpInfoPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendSpInvOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpTpOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        int                                          defDim = 0,
        std::string                                  defPos = ""
    );
    void sendSpMessageOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpMoveOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defPos = "",
        int                                          defOp  = 0
    );
    void
    sendSpLookOperatorPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp, std::string defPos = "");
    void sendSpActionOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1",
        int                                          defOp       = 0
    );
    void sendSpLongActionOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defLong     = "10",
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1",
        int                                          defOp       = 0
    );
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